#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/database/database.hpp"
#include "../../utils/logs/logs.hpp"
#include "../../utils/text/text.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Kick a member from a nation.

    Tasks:
        1) We start with some basic verifications.
            a. Compare the user ID provided and the ID of the command executer. We deny the request if the IDs match.
            b. Try to get some information about the executer in the "nationality" table. If we get no data back, it likely means that the executer is stateless.
            c. Try to get some information about the executer nation using the previously retrieved nation ID.
            d. Try to get some information about the user in the "nationality" table. If we get no data back, it likely means that the user is stateless.
            e. Check that both users are in the same nation by comparing nation IDs. If it does not match, we query some information about user nation for a cleaner error message.
            f. Verify that executer has a rank that is not CITIZEN or MILITARY to grant the permission to kick.
            g. Compare both user ranks, if they match, we deny the kick request.
            h. Compare both user ranks, if user rank is higher than executer rank, we deny the kick request. We do two separated checks for a more accurate error message.
        2) Process the kick request.
            a. Remove the user from the "nationality" table.
            b. Get nation role ID and try to remove it from the user.
            c. Determine the role to remove depending on user rank, and get the ID from the "config" table.
            d. Check that the "gossip" channel and role are valid and send an embed notifying other players of the kick.
            e. Try to remove the rank role of the user.

    Parameters (variable_name / type / description):
        - bot       / dpp::cluster              / Client of the bot with all related information.
        - database  / MYSQL*                    / Database used for the FSB bot and the MineWorld server.
        - event     / dpp::interaction_create_t / All information about the event.

    Returns (type + description):
        No object returned.
*/
void Nation::nation_kick
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    const std::string user_id = std::to_string(std::get<dpp::snowflake>(event.get_parameter("member")));
    const std::string executer_id = std::to_string(event.command.usr.id);

    if (executer_id == user_id)
    {
        event.reply(dpp::message(":prohibited: You can not kick yourself. To leave a nation, run `/nation leave`.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// b. /////////
    Database::Output executer_nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + executer_id + "' LIMIT 1");

    if (executer_id.size() == 0)
    {
        event.reply(dpp::message(":prohibited: You can not kick anyone while being stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// c. /////////
    const std::string executer_nation_id = executer_nationality[0]["nation_id"];
    Database::Output executer_nation = Database::db_query(database, "SELECT display_name, role_id FROM nations WHERE nation_id = '" + executer_nation_id + "' LIMIT 1");

    if (executer_nation.size() == 0)
    {
        Logs::log("Warning: Nation ID " + executer_nation_id + " missing in database -> /nation kick.");
        return event.reply(dpp::message(":prohibited: Something went wrong while retrieving information about nation `" + executer_nation_id + "`.").set_flags(dpp::m_ephemeral));
    }

    ///////// d. /////////
    Database::Output user_nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");
    const std::string display_name = executer_nation[0]["display_name"];

    if (user_nationality.size() == 0)
    {
        event.reply(dpp::message(":prohibited: You can not kick <@" + user_id + "> out of " + display_name + " as they are stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// e. /////////
    const std::string user_nation_id = user_nationality[0]["nation_id"];
    const int user_rank = std::stoi(user_nationality[0]["rank"]);
    const std::string user_rank_name = Text::get_rank(user_rank);

    if (user_nation_id != executer_nation_id)
    {
        Database::Output user_nation = Database::db_query(database, "SELECT display_name FROM nations WHERE nation_id = '" + user_nation_id + "' LINIT 1");

        if (user_nation.size() == 0)
        {
            Logs::log("Warning: Nation ID " + user_nation_id + " missing in database -> /nation kick.");
            return event.reply(dpp::message(":prohibited: You can not kick <@" + user_id + "> as they are not part of " + display_name + ".").set_flags(dpp::m_ephemeral));
        }

        const std::string user_nation_name = user_nation[0]["display_name"];
        return event.reply(dpp::message(":prohibited: You can not kick <@" + user_id + "> out of " + display_name + " as they are part of " + user_nation_name + " as " + user_rank_name + ".").set_flags(dpp::m_ephemeral));
    }

    ///////// f. /////////
    const int executer_rank = std::stoi(executer_nationality[0]["rank"]);
    const std::string executer_rank_name = Text::get_rank(executer_rank);

    if (executer_rank == CITIZEN || executer_rank == MILITARY)
    {
        event.reply(dpp::message(":prohibited: As a " + executer_rank_name + ", you are not allowed to kick any member in " + display_name + ".").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// g. /////////
    if (user_rank == executer_rank)
    {
        event.reply(dpp::message(":prohibited: You can not kick <@" + user_id + "> out of " + display_name + " as you both have the " + user_rank_name + " rank.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// h. /////////
    if (user_rank > executer_rank)
    {
        event.reply(dpp::message(":prohibited: You can not kick <@" + user_id + "> out of " + display_name + " as they have a higher rank than you (" + user_rank_name + " > " + executer_rank_name + ").").set_flags(dpp::m_ephemeral));
        return;
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    Database::db_query(database, "DELETE FROM nationality WHERE user_id = '" + user_id + "'");
    event.reply(dpp::message(":hammer: <@" + user_id + "> has been kicked from " + display_name + ".").set_flags(dpp::m_ephemeral));

    ///////// b. /////////
    const dpp::snowflake guild_id = event.command.guild_id;
    const std::string role_id = executer_nation[0]["role_id"];

    bot.guild_member_remove_role(guild_id, user_id, role_id, [&role_id, &user_id](const dpp::confirmation_callback_t &callback)
    {
        if (callback.is_error())
            Logs::log("Warning: Failed to remove role " + role_id + " to " + user_id + " with error " + callback.get_error().human_readable + " -> /nation kick.");
    });

    ///////// c. /////////
    std::string role_query;

    switch (user_rank)
    {
        case LEADER:
            role_query = "leader_role";
            break;
        case PRIME_MINISTER:
            role_query = "pm_role";
            break;
        case MINISTER:
            role_query = "minister_role";
            break;
        case MILITARY:
            role_query = "military_role";
            break;
        case CITIZEN:
            role_query = "citizen_role";
            break;
    }

    Database::Output config = Database::db_query(database, "SELECT gossip_channel, gossip_role, flags_url " + role_query + " FROM config LIMIT 1");

    if (config.size() == 0)
    {
        Logs::log("Warning: No config data -> /nation kick.");
        return;
    }

    const std::string gossip_channel = config[0]["gossip_channel"];
    const std::string gossip_role = config[0]["gossip_role"];
    const std::string flags_url = config[0]["flags_url"];
    const std::string rank_role = config[0][role_query];

    ///////// d. /////////
    const bool channel_exists = dpp::find_channel(gossip_channel) -> guild_id == guild_id;
    const bool role_exists = dpp::find_role(gossip_role) -> guild_id == guild_id;

    if (channel_exists && role_exists)
    {
        const dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::red)
        .set_title("Citizenship removal")
        .set_thumbnail(flags_url + executer_nation_id + ".png")
        .set_description(user_rank_name + " <@" + user_id + "> was kicked out of " + display_name + " by " + executer_rank_name + " <@" + executer_id + ">.");

        bot.message_create(dpp::message(gossip_channel, "||<@&" + gossip_role + ">").add_embed(embed));
    }
    else Logs::log("Warning: Bad gossip channel " + gossip_channel + " and/or role " + gossip_role + " -> /nation kick.");

    ///////// e. /////////
    bot.guild_member_remove_role(guild_id, user_id, rank_role, [&rank_role, &role_query, &user_id](const dpp::confirmation_callback_t &callback)
    {
        if (callback.is_error())
            Logs::log("Warning: Failed to remove role " + rank_role + " (" + role_query + ") to " + user_id + " with error " + callback.get_error().human_readable + " -> /nation kick.");
    });
}
