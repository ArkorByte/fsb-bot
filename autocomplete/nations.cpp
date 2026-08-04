#include "autocomplete.hpp"

#include "../utils/utils.hpp"

#include <dpp/dpp.h>
#include <map>
#include <mysql/mysql.h>
#include <string>

/*
    Auto complete slash commands that need to display all existing nations.

    Tasks:
        1) Get the "focused value" that is the content typed by the user in the field.
        2) Create a query that gets the list of all nations (25 max due to Discord limits), while adjusting the search using user input.
        3) Create an auto complete entry for each result.
        4) Return the auto complete.

    Parameters:
        - bot      / dpp::cluster       / FSB client data.
        - database / MYSQL*             / FSB + MineWorld database.
        - event    / dpp::form_submit_t / Event information.

    Returns:
        No object returned.
*/
void Autocomplete::nations
(
    dpp::cluster              &bot,
    MYSQL*                    &database,
    const dpp::autocomplete_t &event
)
{
    std::string focused_value;

    for (const dpp::command_option &option : event.options[0].options)
    {
        if (option.focused)
        {
            focused_value = std::get<std::string>(option.value);
            break;
        }
    }

    std::string query = "SELECT nation_id, display_name FROM nations states";

    if (!focused_value.empty())
    {
        focused_value = Utils::Database::sanitize_input(database, focused_value);
        query += " WHERE (states.nation_id LIKE '%" + focused_value + "%' OR states.display_name LIKE '%" + focused_value + "%')";
    }

    query += " ORDER BY states.display_name ASC LIMIT 25";

    Utils::Database::QueryData empty_nations = Utils::Database::db_query(database, query);
    dpp::interaction_response output(dpp::ir_autocomplete_reply);

    for (std::map<std::string, std::string> &empty_nation : empty_nations)
    {
        const std::string nation_id = empty_nation["nation_id"];
        const std::string display_name = empty_nation["display_name"];

        output.add_autocomplete_choice(dpp::command_option_choice(display_name + " (" + nation_id + ")", nation_id));
    }

    bot.interaction_response_create(event.command.id, event.command.token, output);
}
