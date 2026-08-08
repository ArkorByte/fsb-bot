#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Kick a member from a nation.

    Tasks:
        1) Verify user ID.
        2) Verify executer status and rank.
        3) Verify that both nationalities match.
        4) Compare both users ranks.
        5) Delete the user from the "nationality" table.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::nation_kick
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    const std::string user_id = std::to_string(std::get<dpp::snowflake>(event.get_parameter("member")));
    const std::string executer_id = std::to_string(event.command.usr.id);

    if (executer_id == user_id)
    {
        event.reply(dpp::message(":warning: You can not kick yourself. To leave your current nation, run `/nation leave`.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData executer_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + executer_id + "' LIMIT 1");

    if (executer_id.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not kick someone as stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int executer_rank = std::stoi(executer_nationality[0]["rank"]);

    if (executer_rank == CITIZEN || executer_rank == MILITARY)
    {
        event.reply(dpp::message(":warning: You do not have the required permissions to kick another member.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string nation_id = executer_nationality[0]["nation_id"];
    Utils::Database::QueryData user_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "' LIMIT 1");

    if (user_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: This member is either stateless or a member of another nation.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int user_rank = std::stoi(user_nationality[0]["rank"]);

    if (user_rank >= executer_rank)
    {
        event.reply(dpp::message(":warning: You can not kick another government official that has the same rank as you or higher.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::db_query(database, "DELETE FROM nationality WHERE user_id = '" + user_id + "'");
    event.reply(dpp::message(":White_check_mark: This member has been kicked from the nation.").set_flags(dpp::m_ephemeral));
}
