#include "events.hpp"

#include "../utils/logs/logs.hpp"

#include <dpp/dpp.h>

/*
    Event that triggers when a member leaves a server.

    Tasks:
        1) Add the listener to the bot.
        2) Log the departure.

    Parameters (variable_name / type / description):
        - bot / dpp::cluster / Client of the bot with all related information.\

    Returns (type + description):
        No object returned.
*/
void Events::guild_member_remove
(
    dpp::cluster &bot
)
{
    ////////////////// 1) //////////////////
    bot.on_guild_member_remove([&bot](const dpp::guild_member_remove_t &event)
    {
        ////////////////// 2) //////////////////
        Logs::log("Member left -> " + std::to_string(event.removed.id) + ".");
    });
}
