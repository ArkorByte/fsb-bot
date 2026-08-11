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
            b. Try to get some information from the database about this nation using the nation ID provided by the user.
               On failure, it is very likely that the nation does not exist.
            c. Try to get some information about a potential user that has the LEADER rank for this nation ID.
               If we get any data back, it means that the nation already has a Head of State and can not be claimed.
            d. Try to find the user in the "nationality" table of the database.
               If we get any data back, that means that the user is already part of a nation.
            e. If the user already has a nation, we try to find some information about it from the database using the nation ID retrieved from previous query.
               If we get no data back, something went wrong and it is likely an old no-longer-existing ID left behind.
               Otherwise, we get the information to make a clean answer to the user giving their current nation and rank.
        2) If all previous verifications passed, we grant ownership to the user.
            a. Register the user into the "nationality" table as the leader of the nation.
               We also update some statistics for this nation in the "nations" table.
               Finally, we retrieve the nation flag as emoji and make a clean confirmation message. The interaction with the user ends here.
            b. Retrieve the role ID of the nation from a previously made query.
               Check that the role does exist on the server, if it does not, we create a new role using the nation flag and display name.
               Finally, we register the new role ID.
            c. Give the nation role to the user and check for any failure.
               If it fails, we just log the error. It is very likely due to some permission errors.
            d. Get bot config to retrieve "gossip" channel and role, flags URL and the leader role.
               The gossip channel is where we upload some casual information, and the gossip role is the role to mention to notify users.
               Then, flags URL are where we have the flags named by nation IDs stored online, and the leader role is purely cosmetic and informational.
            e. Post an embed informing players about the leadership change if the channel and role do exist.
               If the channel and/or role do not exist, we log the issue.
            f. Try to give the Head of State role to the user.
               If the role do not exist or if it fails, likely due to permission errors, we log the error.

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
    const std::string user_id = std::to_string(event.command.usr.id);
    Database::Output nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (nationality.size() != 0)
    {
        ///////// e. /////////
        const std::string current_nation_id = nationality[0]["nation_id"];
        Database::Output current = Database::db_query(database, "SELECT display_name FROM nation_id = '" + current_nation_id + "' LIMIT 1");

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

    Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + user_id + "', '" + nation_id + "', '" + leader + "', '" + now + "', '" + now + "')");
    Database::db_query(database, "UPDATE nations SET claim_time = '" + now + "', leadership_changes = '" + leadership_changes + "', last_leadership_change = '" + now + "' WHERE nation_id = '" + nation_id + "'");

    event.reply(dpp::message(flag + " You are now the Head of State of " + display_name + ".").set_flags(dpp::m_ephemeral));

    ///////// b. /////////
    const dpp::snowflake guild_id = event.command.guild_id;
    dpp::snowflake role_id = dpp::snowflake(nations[0]["role_id"]);

    if (dpp::find_role(role_id) -> guild_id != guild_id)
    {
        const dpp::role new_role = dpp::role().set_guild_id(guild_id).set_name(flag + " " + display_name).set_color(dpp::colors::white);

        bot.role_create(new_role, [&database, &display_name, &guild_id, &role_id](const dpp::confirmation_callback_t &callback)
        {
            if (callback.is_error())
            {
                Logs::log("Warning: Failed to create role for " + display_name + " with error " + callback.get_error().human_readable + " -> /nation claim.");
                return;
            }

            role_id = std::get<dpp::role>(callback.value).id;
            Database::db_query(database, "UPDATE nations SET role_id = '" + std::to_string(role_id) + "' WHERE guild_id = '" + std::to_string(guild_id) + "'");
        });
    }

    ////////// c. /////////
    bot.guild_member_add_role(guild_id, user_id, role_id, [&role_id, &user_id](const dpp::confirmation_callback_t &callback)
    {
        if (callback.is_error())
            Logs::log("Warning: Failed to give role " + std::to_string(role_id) + " to " + user_id + " with error " + callback.get_error().human_readable + " -> /nation claim.");
    });

    ///////// d. /////////
    Database::Output config = Database::db_query(database, "SELECT gossip_channel, gossip_role, flags_url, leader_role FROM config LIMIT 1");

    if (config.size() == 0)
    {
        Logs::log("Warning: No config for " + std::to_string(guild_id) + " -> /nation claim.");
        return;
    }

    const std::string gossip_channel = config[0]["gossip_channel"];
    const std::string gossip_role = config[0]["gossip_role"];
    const std::string flags_url = config[0]["flags_url"];
    const std::string leader_role = config[0]["leader_role"];

    ///////// e. /////////
    const bool channel_exists = dpp::find_channel(gossip_channel) -> guild_id == guild_id;
    const bool role_exists = dpp::find_role(gossip_role) -> guild_id == guild_id;

    if (channel_exists && role_exists)
    {
        const dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::light_green)
        .set_title("Leadership change")
        .set_thumbnail(flags_url + nation_id + ".png")
        .set_description("<@" + user_id + "> is now the new Head of State of " + display_name + ". This nation has changed of leaders " + leadership_changes + " times since its creation.");

        bot.message_create(dpp::message(gossip_channel, "||<@&" + gossip_role + ">").add_embed(embed));
    }
    else Logs::log("Warning: Bad gossip channel " + gossip_channel + " and/or role " + gossip_role + " -> /nation claim.");

    ///////// f. /////////
    if (dpp::find_role(leader_role) -> guild_id != guild_id)
    {
        bot.guild_member_add_role(guild_id, user_id, leader_role, [&user_id](const dpp::confirmation_callback_t &callback)
        {
            if (callback.is_error())
                Logs::log("Warning: Failed to give leader role to " + user_id + " with error " + callback.get_error().human_readable + " -> /nation claim.");
        });
    }
    else Logs::log("Warning: Bad leader role ID -> /nation claim.");
}
