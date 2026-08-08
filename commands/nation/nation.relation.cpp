#include "nation.hpp"

#include "../../utils/utils.hpp"

#include <algorithm>
#include <cstdint>
#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Set a relation score with another nation.

    Tasks:
        1) Verify nation ID.
        2) Verify user nationality and rank.
        3) Update/Insert relation score in database.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::nation_relation
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    std::string nation_id = std::get<std::string>(event.get_parameter("nation_id"));
    const int64_t score = std::clamp(std::get<int64_t>(event.get_parameter("score")), 0L, 100L);

    nation_id = Utils::Database::sanitize_input(database, nation_id);
    Utils::Database::QueryData nations = Utils::Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":warning: This nation does not exist.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string user_id = std::to_string(event.command.usr.id);
    Utils::Database::QueryData user_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (user_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not do this action as stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int rank = std::stoi(user_nationality[0]["rank"]);

    if (rank == 0)
    {
        event.reply(dpp::message(":warning: You must be a government official to do this action.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string user_nation_id = user_nationality[0]["nation_id"];
    Utils::Database::QueryData current_relationship = Utils::Database::db_query(database, "SELECT * FROM relations WHERE defining_nation = '" + user_nation_id + "' AND targeted_nation = '" + nation_id + "' LIMIT 1");

    if (current_relationship.size() == 0)
        Utils::Database::db_query(database, "INSERT INTO relations (defining_nation, targeted_nation, score) VALUES ('" + user_nation_id + "', '" + nation_id + "', '" + std::to_string(score) + "')");
    else Utils::Database::db_query(database, "UPDATE relations SET score = '" + std::to_string(score) + "' WHERE defining_nation = '" + user_nation_id + "' AND targeted_nation = '" + nation_id + "'");

    event.reply(dpp::message(":white_check_mark: Relation succesfully set to " + std::to_string(score) + "%.").set_flags(dpp::m_ephemeral));
}
