#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/database/database.hpp"
#include "../../utils/logs/logs.hpp"
#include "../../utils/miscellaneous/miscellaneous.hpp"
#include "../../utils/text/text.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Claim a nation.

    Tasks:
        1) We start with some verifications.
            a. Sanitize user input to prevent SQL injections as much as possible.
            b. Try to get some information about the nation from the database to check that it exists.
            c. Try to get some information about a potential user that has the LEADER rank for this nation ID.
               If we get any data back, it means that the nation already has a Head of State and can not be claimed.
            d. Try to find the user in the "nationality" table of the database. If we get some data back, the user is already part of a nation.
            e. Try to find some information about the user nation for a cleaner and more detailed error message.
        2) Grant ownership to the user.
            a. Register the user into the "nationality" table as the leader of the nation, update nation statistics and get some information for a cleaner confirmation message.
            b. Retrieve the nation role ID of the nation, we verify that the role exists. If it does not, we create it and register the new role ID.
            c. Try to give the nation role to the user.
            d. Get bot config to retrieve essentials information for later.
            e. Check that the "gossip" channel and role are valid, and send an embed notifying other players of the leadership change.

    Parameters (variable_name / type / description):
        - bot       / dpp::cluster              / Client of the bot with all related information.
        - database  / MYSQL*                    / Database used for the FSB bot and the MineWorld server.
        - event     / dpp::interaction_create_t / All information about the event.

    Returns (type + description):
        No object returned.
*/
void Nation::claim_nation
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    ////////////////// 1) //////////////////
    ///////// a. //////////
    const std::string nation_id = Database::sanitize_input(database, std::get<std::string>(event.get_parameter("nation_id")));

    ///////// b. /////////
    Database::Output nations = Database::db_query(database, "SELECT display_name, role_id, leadership_changes FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":prohibited: Nation `" + nation_id + "` does not exist.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// c. /////////
    const std::string display_name = nations[0]["display_name"];
    const std::string leader = std::to_string(LEADER);

    Database::Output leadership = Database::db_query(database, "SELECT user_id FROM nationality WHERE nation_id = '" + nation_id + "' AND rank = '" + leader + "' LIMIT 1");

    if (leadership.size() != 0)
    {
        event.reply(dpp::message(":prohibited: You can not claim " + display_name +  " as it already has <@" + leadership[0]["user_id"] + "> as Head of State.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// d. /////////
    const dpp::snowflake user_id = event.command.usr.id;
    Database::Output nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + std::to_string(user_id) + "' LIMIT 1");

    if (nationality.size() != 0)
    {
        ///////// e. /////////
        const std::string current_nation_id = nationality[0]["nation_id"];
        Database::Output current = Database::db_query(database, "SELECT display_name FROM nations WHERE nation_id = '" + current_nation_id + "' LIMIT 1");

        if (current.size() == 0)
        {
            Logs::log("Warning: Bad nation_id " + current_nation_id + " -> /nation claim.");
            return event.reply(dpp::message(":prohibited: You already part of another nation.").set_flags(dpp::m_ephemeral));
        }

        const std::string current_name = current[0]["display_name"];
        const std::string rank = Text::get_rank(std::stoi(nationality[0]["rank"]));

        return event.reply(dpp::message(":prohibited: You are already part of " + current_name + " as " + rank + ".").set_flags(dpp::m_ephemeral));
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    const std::string flag = Text::get_nation_flag(nation_id);
    const std::string leadership_changes = std::to_string(std::stoll(nations[0]["leadership_changes"]) + 1);
    const std::string now = std::to_string(Miscellaneous::get_current_timestamp());

    Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + std::to_string(user_id) + "', '" + nation_id + "', '" + leader + "', '" + now + "', '" + now + "')");
    Database::db_query(database, "UPDATE nations SET claim_time = '" + now + "', leadership_changes = '" + leadership_changes + "', last_leadership_change = '" + now + "' WHERE nation_id = '" + nation_id + "'");

    event.reply(dpp::message(flag + " You are now the Head of State of " + display_name + ".").set_flags(dpp::m_ephemeral));

    ///////// b. /////////
    const dpp::snowflake guild_id = event.command.guild_id;
    const dpp::snowflake role_id = dpp::snowflake(nations[0]["role_id"]);

    bot.guild_member_add_role(guild_id, user_id, role_id, [&bot, &database, display_name, flag, guild_id, nation_id, role_id, user_id](const dpp::confirmation_callback_t &callback)
    {
        if (callback.is_error())
        {
            Logs::log("Warning: Failed to give role " + std::to_string(role_id) + " to " + std::to_string(user_id) + " with error " + callback.get_error().human_readable + " -> /nation claim.");

            ///////// c. /////////
            const dpp::role new_role = dpp::role().set_guild_id(guild_id).set_name(flag + " " + display_name).set_color(dpp::colors::white);

            bot.role_create(new_role, [&bot, &database, display_name, guild_id, nation_id, user_id](const dpp::confirmation_callback_t &callback)
            {
                if (callback.is_error())
                {
                    Logs::log("Warning: Failed to create role for " + display_name + " with error " + callback.get_error().human_readable + " -> /nation claim.");
                    return;
                }

                const dpp::snowflake role_id = std::get<dpp::role>(callback.value).id;
                Database::db_query(database, "UPDATE nations SET role_id = '" + std::to_string(role_id) + "' WHERE nation_id = '" + nation_id + "'");

                ///////// d. /////////
                bot.guild_member_add_role(guild_id, user_id, role_id, [&bot, role_id, user_id](const dpp::confirmation_callback_t &callback)
                {
                    if (callback.is_error())
                        Logs::log("Warning: Failed to give role " + std::to_string(role_id) + " to " + std::to_string(user_id) + " with error " + callback.get_error().human_readable + " -> /nation claim.");
                });
            });
        }
    });

    ///////// e. /////////
    Database::Output config = Database::db_query(database, "SELECT gossip_channel, gossip_role, flags_url FROM config LIMIT 1");

    if (config.size() == 0)
    {
        Logs::log("Warning: No config data -> /nation claim.");
        return;
    }

    const dpp::snowflake gossip_channel = dpp::snowflake(config[0]["gossip_channel"]);
    const std::string gossip_role = config[0]["gossip_role"];
    const std::string flags_url = config[0]["flags_url"];

    ///////// f. /////////
    const dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::light_green)
    .set_title("New Leadership")
    .set_thumbnail(flags_url + nation_id + ".png")
    .set_description("Stateless <@" + std::to_string(user_id) + "> claimed the leadership of " + display_name + " and is now its new Head of State.");

    bot.message_create
    (
        dpp::message(gossip_channel, "||<@&" + gossip_role + ">||").add_embed(embed),
        [gossip_channel](const dpp::confirmation_callback_t &callback)
        {
            if (callback.is_error())
                Logs::log("Warning: Failed to send message in " + std::to_string(gossip_channel) + " with error " + callback.get_error().human_readable + " -> /nation claim.");
        }
    );
}
