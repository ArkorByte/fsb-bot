#include "nation.hpp"

#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Invite a user into a nation.

    Tasks:
        1) Verify user ID.
        2) Verify executer is not stateless.
        3) Verify executer nation ID.
        4) Verify nation is not closed or opened (useless to invite).
        5) Verify executer meets invite permissions.
        6) Verify user does not already have pending invitation.
        7) Register pending invitation in database.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::nation_invite
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    const std::string user_id = std::to_string(std::get<dpp::snowflake>(event.get_parameter("player")));
    const std::string executer_id = std::to_string(event.command.usr.id);

    if (user_id == executer_id)
    {
        event.reply(dpp::message(":warning: You can not invite yourself.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData executer_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + executer_id + "' LIMIT 1");

    if (executer_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not invite anyone while being stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string nation_id = executer_nationality[0]["nation_id"];
    Utils::Database::QueryData nations = Utils::Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":warning: Something went wrong!").set_flags(dpp::m_ephemeral));
        Utils::Logs::log("FIX NEEDED: Nation ID " + nation_id + " is missing in database -> /nation invite.");
        return;
    }

    enum JoinCondition
    {
        OPENED = 0,
        ON_INVITATION = 1,
        CLOSED = 2
    };

    const int join_condition = std::stoi(nations[0]["join_condition"]);

    if (join_condition == OPENED)
    {
        event.reply(dpp::message(":warning: This nation is opened and therefore anyone can already join at anytime.").set_flags(dpp::m_ephemeral));
        return;
    }

    if (join_condition == CLOSED)
    {
        event.reply(dpp::message(":warning: This nation is currently closed. No one can join it at this time.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int invite_permission = std::stoi(nations[0]["invite_permission"]);
    const int executer_rank = std::stoi(executer_nationality[0]["rank"]);

    if (executer_rank < invite_permission)
    {
        event.reply(dpp::message(":warning: You are not allowed to send invitations due to government laws.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData is_already_invited = Utils::Database::db_query(database, "SELECT * FROM invitations WHERE user_id = '" + user_id + "' LIMIT 1");

    if (is_already_invited.size() != 0)
    {
        event.reply(dpp::message(":warning: This user already has a pending invitation for this nation.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::db_query(database, "INSERT INTO invitations (user_id, nation_id, invited_by, creation_time) VALUES ('" + user_id + "', '" + nation_id + "', '" + executer_id + "', '" + std::to_string(Utils::Miscellaneous::get_current_timestamp()) + "')");
    event.reply(dpp::message(":white_check_mark: An invitation is now pending for this user. They have 24 hours to run `/nation join` before the invitation expires.").set_flags(dpp::m_ephemeral));
}
