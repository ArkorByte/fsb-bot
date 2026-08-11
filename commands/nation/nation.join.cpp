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
    Join a nation.

    Tasks:
        1) We start with some basic verifications.
            a. Sanitize user input to prevent SQL injections as much as possible.
            b. Try to get some information from the database in the "nations" table using the nation ID provided by the user.
               If we get no data back, it is likely due to a wrong nation ID.
            c. From the data previously retrieved, we verify that the nation is not set as closed by the nation government.
            d. Try to get some information about the user in the "nationality" table.
               If we get some data, it means that the user is already a member of a nation.
            e. Try to get some information about the nation that the user is part of to make a cleaner message with nation name and user rank.
               If we get no data back, we make a generic message and log the error.
        2) If all basic checks validated, we process the joining request.
            a. If the nation is set as opened by nation government, we immediatly register the user as citizen of the nation in the database and send a clean message.
            b. If the nation is set as on invitation, we verify that the user do actually have one.
            c. If the user has an invitation, we verify that the invitation has not expired.
               On expiration, we directly delete the invitation from the database based on user ID and nation ID.
            d. Get bot config to retrieve "gossip" channel and role, flags URL and the citizen role.
               The gossip channel is where we upload some casual information, and the gossip role is the role to mention to notify users.
               Then, flags URL are where we have the flags named by nation IDs stored online, and the citizen role is purely cosmetic and informational.
            e. Post an embed informing players about the leadership change if the channel and role do exist.
               We also precise who invited the user if it was on invitation only.
            f. Try to give the citizen role to the user.
               If the role do not exist or if it fails, likely due to permission errors, we log the error.

    Parameters (variable_name / type / description):
        - bot       / dpp::cluster              / Client of the bot with all related information.
        - database  / MYSQL*                    / Database used for the FSB bot and the MineWorld server.
        - event     / dpp::interaction_create_t / All information about the event.

    Returns (type + description):
        No object returned.
*/
void Nation::join_nation
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    const std::string nation_id = Database::sanitize_input(database, std::get<std::string>(event.get_parameter("nation_id")));

    ///////// b. /////////
    Database::Output nations = Database::db_query(database, "SELECT display_name, join_condition FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":prohibited: Nation `" + nation_id + "` does not exist.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// c. /////////
    const int join_condition = std::stoi(nations[0]["join_condition"]);
    const std::string display_name = nations[0]["display_name"];

    if (join_condition == CLOSED)
    {
        event.reply(dpp::message(":prohibited: " + display_name + " is currently closed to anyone.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// d. /////////
    const std::string user_id = std::to_string(event.command.usr.id);
    Database::Output nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

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
        const std::string rank = current[0]["rank"];

        return event.reply(dpp::message(":prohibited: You are already part of " + current_name + " as " + rank + ".").set_flags(dpp::m_ephemeral));
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    const std::string now = std::to_string(Miscellaneous::get_current_timestamp());
    const std::string citizen = std::to_string(CITIZEN);
    const std::string flag = Text::get_nation_flag(nation_id);

    if (join_condition == OPENED)
    {
        Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + user_id + "', '" + nation_id + "', '" + citizen + "', '" + now + "', '" + now + "')");
        event.reply(dpp::message(flag + " You are now a citizen of " + display_name + ".").set_flags(dpp::m_ephemeral));
    }

    ///////// b. /////////
    std::string inviter_id;

    if (join_condition == ON_INVITATION)
    {
        Database::Output invitation = Database::db_query(database, "SELECT invited_by, creation_time FROM invitations WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "' LIMIT 1");

        if (invitation.size() == 0)
        {
            event.reply(dpp::message(":prohibited: You need an invitation to join " + display_name + ".").set_flags(dpp::m_ephemeral));
            return;
        }

        ///////// c. /////////
        const int64_t invitation_time = std::stoll(invitation[0]["creation_time"]);
        const int expiration = 86400; // 24 hours.

        if (invitation_time + expiration < std::stoll(now))
        {
            Database::db_query(database, "DELETE FROM invitations WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "'");
            return event.reply(dpp::message(":prohibited: Your invitation to join " + display_name + " has expired.").set_flags(dpp::m_ephemeral));
        }

        Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + user_id + "', '" + nation_id + "', '" + citizen + "', '" + now + "', '" + now + "')");
        event.reply(dpp::message(flag + " You are now a citizen of " + display_name + ".").set_flags(dpp::m_ephemeral));
        inviter_id = invitation[0]["invited_by"];
    }

    ///////// d. /////////
    Database::Output config = Database::db_query(database, "SELECT gossip_channel, gossip_role, citizen_role LIMIT 1");
    const dpp::snowflake guild_id = event.command.guild_id;

    if (config.size() == 0)
    {
        Logs::log("Warning: No config for " + std::to_string(guild_id) + " -> /nation join.");
        return;
    }

    const std::string gossip_channel = config[0]["gossip_channel"];
    const std::string gossip_role = config[0]["gossip_role"];
    const std::string flags_url = config[0]["flags_url"];
    const std::string citizen_role = config[0]["citizen_role"];

    ///////// e. /////////
    const bool channel_exists = dpp::find_channel(gossip_channel) -> guild_id == guild_id;
    const bool role_exists = dpp::find_role(gossip_role) -> guild_id == guild_id;

    if (channel_exists && role_exists)
    {
        std::string was_invited;

        if (join_condition == ON_INVITATION)
        {
            Database::Output inviter = Database::db_query(database, "SELECT rank WHERE user_id = '" + inviter_id + "' AND nation_id = '" + nation_id + "' LIMIT 1");

            if (inviter.size() != 0)
            {
                const std::string rank = Text::get_rank(std::stoi(inviter[0]["rank"]));
                was_invited = " They were invited by " + rank + " <@" + inviter_id + ">.";
            }
        }

        const dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::light_green)
        .set_title("New citizen")
        .set_thumbnail(flags_url + nation_id + ".png")
        .set_description("<@" + user_id + "> just received his citizenship from " + display_name + "." + was_invited);

        bot.message_create(dpp::message(gossip_channel, "||<@&" + gossip_role + ">").add_embed(embed));
    }
    else Logs::log("Warning: Bad gossip channel " + gossip_channel + " and/or role " + gossip_role + " -> /nation join.");

    ///////// f. /////////
    if (dpp::find_role(citizen_role) -> guild_id != guild_id)
    {
        bot.guild_member_add_role(guild_id, user_id, citizen_role, [&user_id](const dpp::confirmation_callback_t &callback)
        {
            if (callback.is_error())
                Logs::log("Warning: Failed to give citizen role to " + user_id + " with error " + callback.get_error().human_readable + " -> /nation join.");
        });
    }
    else Logs::log("Warning: Bad citizen role ID -> /nation join.");
}
