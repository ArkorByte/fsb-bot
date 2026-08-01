#include "events.hpp"

#include "../buttons/buttons.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>

/*
    Event that triggers when a button was clicked.

    Tasks:
        1) Add the listener to the bot.
        2) Handle request depending on custom ID.

    Parameters:
        - bot      / dpp::cluster / FSB client data.
        - database / MYSQL*       / FSB + MineWorld database.

    Returns:
        No object returned.
*/
void Events::button_click
(
    dpp::cluster &bot,
    MYSQL*       &database
)
{
    bot.on_button_click([&bot, &database](dpp::button_click_t event)
    {
        if (event.custom_id == "journalism_censor" || event.custom_id == "journalism_blacklist")
            Buttons::journalism_censor_button(bot, event.custom_id, database, event);
    });
}
