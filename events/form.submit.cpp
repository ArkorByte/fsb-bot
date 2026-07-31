#include "events.hpp"

#include "../modals/modals.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>

/*

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
