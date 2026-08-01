#include "events.hpp"

#include "../modals/modals.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>

/*
    Event that triggers when a modal has been submitted.

    Tasks:
        1) Add the listener to the bot.
        2) Handle request depending on custom ID.

    Parameters:
        - bot      / dpp::cluster / FSB client data.
        - database / MYSQL*       / FSB + MineWorld database.

    Returns:
        No object returned.
*/
void Events::form_submit
(
    dpp::cluster &bot,
    MYSQL*       &database
)
{
    bot.on_form_submit([&bot, &database](const dpp::form_submit_t &event)
    {
        if (event.custom_id == "journalism_modal")
            Modals::journalism(bot, database, event);
    });
}
