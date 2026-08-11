#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/database/database.hpp"
#include "../../utils/logs/logs.hpp"
#include "../../utils/miscellaneous/miscellaneous.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Invite a player into a nation.

    Tasks:
        1) We start with some basic verifications.
            a. Get the "player" parameter from the slash command and compare the executer and provided IDs.
            b. Try to get some information about the executer in the "nationality" table.
               If no data comes out, it means that the user is stateless and can not send any invitation.
            c. Try to get some information about the executer nation using the retrieved nation ID from previous request.
               If we get no data back, something went wrong and it is likely an old no-longer-existing ID left behind.
            d. Verify the "join condition" configured by the government of the nation.
               If the nation is set as closed, no one is allowed to join and we disallow invitations.
               If the nation is set as opened, anyone can join at anytime. We disallow invitations to save server resources since it is useless.
        2) As the server is on invitation mode, we process the request.
            a. Verify that executer meets permission requirements to send invitations according to nation government config.
            b. Check that the user do not already have a pending invitation for them to join the nation.
               If we do have an invitation pending, we verify that it did not expire. If it did not, we cancel the invitation process.
            c. Register the invitation into the database and send a confirmation message to the user.

    Parameters (variable_name / type / description):
        - bot       / dpp::cluster              / Client of the bot with all related information.
        - database  / MYSQL*                    / Database used for the FSB bot and the MineWorld server.
        - event     / dpp::interaction_create_t / All information about the event.

    Returns (type + description):
        No object returned.
*/
void Nation::nation_invite
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    const std::string user_id = std::to_string(std::get<dpp::snowflake>(event.get_parameter("player")));
    const std::string executer_id = std::to_string(event.command.usr.id);

    if (user_id == executer_id)
    {
        event.reply(dpp::message(":prohibited: You can not send an invitation to yourself.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// b. /////////
    Database::Output nationality = Database::db_query(database, "SELECT nation_id, rank FROM nationality WHERE user_id = '" + executer_id + "' LIMIT 1");

    if (nationality.size() == 0)
    {
        event.reply(dpp::message(":prohibited: You can not send an invitation to anyone while being stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// c. /////////
    const std::string nation_id = nationality[0]["nation_id"];
    Database::Output nations = Database::db_query(database, "SELECT display_name, join_condition, invite_permission FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":prohibited: Something went wrong while retrieving information about nation `" + nation_id + "`.").set_flags(dpp::m_ephemeral));
        return Logs::log("Warning: Nation ID " + nation_id + " missing in database -> /nation invite.");
    }

    ///////// d. /////////
    const int join_condition = std::stoi(nations[0]["join_condition"]);
    const std::string display_name = nations[0]["display_name"];

    if (join_condition == OPENED)
    {
        event.reply(dpp::message(":prohibited: " + display_name + " is already opened for anyone to join.").set_flags(dpp::m_ephemeral));
        return;
    }

    if (join_condition == CLOSED)
    {
        event.reply(dpp::message(":prohibited: " + display_name + " is currently closed to anyone.").set_flags(dpp::m_ephemeral));
        return;
    }

    ////////////////// 2) //////////////////
    ///////// a. /////////
    const int permission = std::stoi(nations[0]["invite_permission"]);
    const int rank = std::stoi(nationality[0]["rank"]);

    if ((permission == GOV_ONLY && rank < MINISTER) || (permission == PM_MINIMUM && rank < PRIME_MINISTER) || (permission == LEADER_ONLY && rank < LEADER))
    {
        event.reply(dpp::message(":prohibited: You are not allowed to send invitations due to ongoing government immigration laws.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// b. /////////
    Database::Output invitation = Database::db_query(database, "SELECT creation_time FROM invitations WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "' LIMIT 1");
    const int64_t now = Miscellaneous::get_current_timestamp();

    if (invitation.size() != 0)
    {
        const int64_t invitation_time = std::stoll(invitation[0]["creation_time"]);
        const int expiration = 86400; // 24 hours.

        if (invitation_time + expiration > now)
        {
            event.reply(dpp::message(":prohibited: <@" + user_id + "> already has a pending invitation to join " + display_name + ".").set_flags(dpp::m_ephemeral));
            return;
        }
        else Database::db_query(database, "DELETE FROM invitations WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "'");
    }

    ///////// c. /////////
    Database::db_query(database, "INSERT INTO invitations (user_id, nation_id, invited_by, creation_time) VALUES ('" + user_id + "', '" + nation_id + "', '" + executer_id + "', '" + std::to_string(now) + "')");
    event.reply(dpp::message(":envelope: An invitation is now pending for <@" + user_id + "> to join " + display_name + ".\nThey have 24 hours to run `/nation join` before it expires.").set_flags(dpp::m_ephemeral));
}
