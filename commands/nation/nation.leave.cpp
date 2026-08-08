#include "nation.hpp"

#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Leave a nation.

    Tasks:
        1) Verify that user is in a nation.
        2) Get member count for user nation.
        3) Verify rank and deny leaving if user is head of state and member count > 1.
        4) Delete the user from the "nationality" table.

    Parameters:
        - bot       / dpp::cluster       / FSB client data.
        - database  / MYSQL*             / FSB + MineWorld database.
        - event     / dpp::form_submit_t / Event information.

    Returns:
        No object returned.
*/
void Nation::leave_nation
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    const std::string user_id = std::to_string(event.command.usr.id);
    Utils::Database::QueryData user_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (user_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not leave any nation as stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string nation_id = user_nationality[0]["nation_id"];
    Utils::Database::QueryData nation_members = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' AND nation_id = '" + nation_id + "'");

    const int member_count = nation_members.size();
    const int rank = std::stoi(user_nationality[0]["rank"]);
    const int LEADER = 3;

    if (rank == LEADER && member_count > 1)
    {
        event.reply(dpp::message(":warning: You can not leave a populated nation as the head of state. Either transfer ownership to another member using `/nation rank` or remove all other members using `/nation kick` in order to leave.").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::db_query(database, "DELETE FROM nationality WHERE user_id = '" + user_id + "'");
    event.reply(dpp::message(":white_check_mark: You left your nation.").set_flags(dpp::m_ephemeral));
}
