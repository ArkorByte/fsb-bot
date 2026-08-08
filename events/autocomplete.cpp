#include "events.hpp"

#include "../autocomplete/autocomplete.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Event that triggers when a slash command using auto complete is being used.

    Tasks:
        1) Add the listener to the bot.
        2) Handle request depending on (sub)command names.

    Parameters:
        - bot      / dpp::cluster / FSB client data.
        - database / MYSQL*       / FSB + MineWorld database.

    Returns:
        No object returned.
*/
void Events::autocomplete
(
    dpp::cluster &bot,
    MYSQL*       &database
)
{
    bot.on_autocomplete([&bot, &database](const dpp::autocomplete_t &event)
    {
        if (event.name == "nation")
        {
            const std::string subcommand = event.options[0].name;

            if (subcommand == "claim")
                Autocomplete::empty_nations(bot, database, event);
            else if (subcommand == "info" || subcommand == "join" || subcommand == "relation")
                Autocomplete::nations(bot, database, event);
            else if (subcommand == "rank")
                Autocomplete::ranks(bot, event);
        }
    });
}
