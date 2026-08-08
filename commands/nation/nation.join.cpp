#include "nation.hpp"

#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Join a nation.

    Tasks:
        1) Verify nation ID.
        2) Verify that nation is not closed.
        3) If nation opened, directly give nationality as citizen.
        4) If on invitation, verify that user has pending invitation.
        5) If invited, give nationality as citizen.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::join_nation
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    std::string nation_id = std::get<std::string>(event.get_parameter("nation_id"));

    nation_id = Utils::Database::sanitize_input(database, nation_id);
    Utils::Database::QueryData nations = Utils::Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":warning: This nation does not exist.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int join_condition = std::stoi(nations[0]["join_condition"]);
    enum JoinCondition { OPENED = 1, CLOSED = 2 };

    if (join_condition == CLOSED)
    {
        event.reply(dpp::message(":warning: This nation is currently closed. No one can join it at this time.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string user_id = std::to_string(event.command.usr.id);
    Utils::Database::QueryData user_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (user_nationality.size() != 0)
    {
        event.reply(dpp::message(":warning: You are already a member of a nation. To proceed, leave your current nation first.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string display_name = nations[0]["display_name"];
    const int64_t now = Utils::Miscellaneous::get_current_timestamp();

    if (join_condition == OPENED)
    {
        Utils::Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + user_id + "', '" + nation_id + "', 0, '" + std::to_string(now) + "', '" + std::to_string(now) + "')");
        event.reply(dpp::message(":white_check_mark: You are now a member of " + display_name + ".").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData is_invited = Utils::Database::db_query(database, "SELECT * FROM invitations WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "' LIMIT 1");

    if (is_invited.size() == 0)
    {
        event.reply(dpp::message(":warning: You need an invitation to join this nation. If one was already made for you, it may have expired.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + user_id + "', '" + nation_id + "', 0, '" + std::to_string(now) + "', '" + std::to_string(now) + "')");
    event.reply(dpp::message(":white_check_mark: You are now a member of " + display_name + ".").set_flags(dpp::m_ephemeral));
}
