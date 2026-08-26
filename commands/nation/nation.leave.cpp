#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/database/database.hpp"
#include "../../utils/logs/logs.hpp"
#include "../../utils/text/text.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Leave a nation.

    Tasks:
        1) We do some basic verifications.
            a. Try to get some information about the user in the "nationality" table. If we have no data back, it likely means that the user is stateless.
            b. Try to get some information about the user nation.
            c. Get the nation member count, and check the user rank. We deny the user to leave if they are the Head of State and any other member in the nation.
        2) Process the leaving request.
            a. Remove the user from the nation in database.
            b. Get nation role ID and try to remove it from the user.
            c. Determine the role to remove depending on user rank, and get some required IDs from the "config" table.
            d. Check that the "gossip" channel and role are valid, and send an embed notifying other players that the user left.

    Parameters (variable_name / type / description):
        - bot       / dpp::cluster              / Client of the bot with all related information.
        - database  / MYSQL*                    / Database used for the FSB bot and the MineWorld server.
        - event     / dpp::interaction_create_t / All information about the event.

    Returns (type + description):
        No object returned.
*/
void Nation::leave_nation
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    const std::string user_id = std::to_string(event.command.usr.id);
    Database::Output nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (nationality.size() == 0)
    {
        event.reply(dpp::message(":prohibited: You can not perform this action white being stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// b. /////////
    const std::string nation_id = nationality[0]["nation_id"];
    Database::Output nations = Database::db_query(database, "SELECT display_name, role_id FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        Logs::log("Warning: Nation ID " + nation_id + " missing in database -> /nation leave.");
        return event.reply(dpp::message(":prohibited: Something went wrong while retrieving information about nation `" + nation_id + "`.").set_flags(dpp::m_ephemeral));
    }

    ///////// c. /////////
    Database::Output nation_members = Database::db_query(database, "SELECT 1 FROM nationality WHERE nation_id = '" + nation_id + "'");

    const std::string display_name = nations[0]["display_name"];
    const int member_count = nation_members.size();
    const int rank = std::stoi(nationality[0]["rank"]);

    if (rank == LEADER && member_count > 1)
    {
        event.reply(dpp::message(
            ":prohibited: You can not leave " + display_name + " as the Head of State as there are " + std::to_string(member_count - 1) + " other member(s). " +
            "Either transfer ownership using `/nation rank` or remove all other members using `/nation kick` in order to leave."
        ).set_flags(dpp::m_ephemeral));
        return;
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    Database::db_query(database, "DELETE FROM nationality WHERE user_id = '" + user_id + "'");
    const std::string rank_name = Text::get_rank(rank);

    event.reply(dpp::message(":wave: You left " + display_name + " and lost your " + rank_name + " rank.").set_flags(dpp::m_ephemeral));

    ///////// b. /////////
    const dpp::snowflake guild_id = event.command.guild_id;
    const std::string role_id = nations[0]["role_id"];

    bot.guild_member_remove_role(guild_id, user_id, role_id, [&role_id, &user_id](const dpp::confirmation_callback_t &callback)
    {
        if (callback.is_error())
            Logs::log("Warning: Failed to remove role " + role_id + " to " + user_id + " with error " + callback.get_error().human_readable + " -> /nation leave.");
    });

    ///////// c. /////////
    Database::Output config = Database::db_query(database, "SELECT gossip_channel, gossip_role, flags_url FROM config LIMIT 1");

    if (config.size() == 0)
    {
        Logs::log("Warning: No config data -> /nation leave.");
        return;
    }

    const std::string gossip_channel = config[0]["gossip_channel"];
    const std::string gossip_role = config[0]["gossip_role"];
    const std::string flags_url = config[0]["flags_url"];

    ///////// d. /////////
    const bool channel_exists = dpp::find_channel(gossip_channel) -> guild_id == guild_id;
    const bool role_exists = dpp::find_role(gossip_role) -> guild_id == guild_id;

    if (!channel_exists && !role_exists)
    {
        Logs::log("Warning: Bad gossip channel " + gossip_channel + " and/or role " + gossip_role + " -> /nation leave.");
        return;
    }

    const dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::red)
    .set_title("Citizenship Renounced")
    .set_thumbnail(flags_url + nation_id + ".png")
    .set_description(rank_name + " <@" + user_id + "> just left " + display_name + " and are now stateless.");

    bot.message_create(dpp::message(gossip_channel, "||<@&" + gossip_role + ">||").add_embed(embed));
}
