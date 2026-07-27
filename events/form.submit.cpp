#include "events.hpp"

#include "../modals/modals.hpp"

#include <dpp/dpp.h>

/*

*/
void Events::form_submit
(
    dpp::cluster &bot
)
{
    bot.on_form_submit([&bot](const dpp::form_submit_t &event)
    {
        if (event.custom_id == "journalism_modal")
            Modals::journalism(bot, event);
    });
}
