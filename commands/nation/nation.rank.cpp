#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Change the rank of a member of a nation.

    Tasks:
        1) Verify new rank value.
        2) Verify user ID.
        3) Verify both user nationalities and compare nation IDs.
        4) Verify that executer has higher rank.
        5) If new rank is PM or Head of State, we demote the current member holding the role.
        6) Update user rank in database.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::nation_rank
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    const std::string user_id = std::to_string(std::get<dpp::snowflake>(event.get_parameter("member")));
    const int64_t new_rank = std::get<int64_t>(event.get_parameter("new_rank"));

    if (new_rank != CITIZEN && new_rank != MILITARY && new_rank != MINISTER && new_rank != PRIME_MINISTER && new_rank != LEADER)
    {
        event.reply(dpp::message(":warning: Invalid new rank provided.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string executer_id = std::to_string(event.command.usr.id);

    if (user_id == executer_id)
    {
        event.reply(dpp::message(":warning: You can not change your own rank.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData user_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");
    Utils::Database::QueryData executer_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + executer_id + "' LIMIT 1");

    if (user_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not change the rank of a stateless person.").set_flags(dpp::m_ephemeral));
        return;
    }

    if (executer_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not perform this action as stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string user_nation_id = user_nationality[0]["nation_id"];
    const std::string executer_nation_id = executer_nationality[0]["nation_id"];

    if (user_nation_id != executer_nation_id)
    {
        event.reply(dpp::message(":warning: You can not change the rank of members of other nations.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int user_rank = std::stoi(user_nationality[0]["rank"]);
    const int executer_rank = std::stoi(executer_nationality[0]["rank"]);

    if (user_rank >= executer_rank)
    {
        event.reply(dpp::message(":warning: You can not change the rank of a member that has the same rank as you or higher.").set_flags(dpp::m_ephemeral));
        return;
    }

    if (new_rank == PRIME_MINISTER)
        Utils::Database::db_query(database, "UPDATE nationality SET rank = '" + std::to_string(CITIZEN) + "' WHERE nation_id = '" + user_nation_id + "' AND rank = '" + std::to_string(PRIME_MINISTER) + "'");

    if (new_rank == LEADER)
        Utils::Database::db_query(database, "UPDATE nationality SET rank = '" + std::to_string(CITIZEN) + "' WHERE nation_id = '" + user_nation_id + "' AND rank = '" + std::to_string(LEADER) + "'");

    Utils::Database::db_query(database, "UPDATE nationality SET rank = '" + std::to_string(new_rank) + "' WHERE user_id = '" + user_id + "'");
    const std::string rank = Utils::Text::get_rank(new_rank);

    event.reply(dpp::message(":white_check_mark: Rank " + rank + " was given to this user. Note that if the Prime Minister or Head of State ranks were given, the current members holding these ranks were demoted to Citizen.").set_flags(dpp::m_ephemeral));
}
