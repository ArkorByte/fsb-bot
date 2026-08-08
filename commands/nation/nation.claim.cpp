#include "nation.hpp"

#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Claim a nation that does not have any head of state.

    Tasks:
        1) Verify nation ID.
        2) Try to get a head of state for the targeted nation.
        3) Verify that user is stateless.
        4) Set the user as head of state and update stats.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::claim_nation
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
        event.reply(dpp::message(":warning: This nation does not exist!").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData leadership = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE nation_id = '" + nation_id + "' AND rank = '3' LIMIT 1");

    if (leadership.size() != 0)
    {
        event.reply(dpp::message(":warning: You can not claim a nation that already has a head of state!").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string user_id = std::to_string(event.command.usr.id);
    const int64_t leadership_changes = std::stoll(nations[0]["leadership_changes"]) + 1;
    const std::string now = std::to_string(Utils::Miscellaneous::get_current_timestamp());

    Utils::Database::QueryData nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (nationality.size() != 0)
    {
        const std::string nation_id = nationality[0]["nation_id"];

        if (!nation_id.empty())
        {
            event.reply(dpp::message(":warning: You are already a member of a nation! To proceed, leave your current state.").set_flags(dpp::m_ephemeral));
            return;
        }
        else Utils::Database::db_query(database, "UPDATE nationality SET nation_id = '" + nation_id + "', rank = 3, last_rank_update = '" + now + "', joining_time = '" + now + "' WHERE user_id = '" + user_id + "'");
    }
    else Utils::Database::db_query(database, "INSERT INTO nationality (user_id, nation_id, rank, last_rank_update, joining_time) VALUES ('" + user_id + "', '" + nation_id + "', 3, '" + now + "', '" + now + "')");

    const std::string display_name = nations[0]["display_name"];

    Utils::Database::db_query(database, "UPDATE nations SET claim_time = '" + now + "', leadership_changes = '" + std::to_string(leadership_changes) + "', last_leadership_change = '" + now + "' WHERE nation_id = '" + nation_id + "'");
    event.reply(dpp::message(":white_check_mark: You are now the head of state of " + display_name + "!").set_flags(dpp::m_ephemeral));
}
