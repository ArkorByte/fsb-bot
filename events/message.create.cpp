#include "events.hpp"

#include <dpp/dpp.h>

/*
    Event that triggers when a new message is sent on Discord.

    Tasks:
        1) Add the listener to the bot.
        2) Handle specific messages.

    Parameters:
        - bot / dpp::cluster / FSB client data.

    Returns:
        No object returned.
*/
void Events::message_create
(
    dpp::cluster &bot
)
{
    bot.on_message_create([&bot](const dpp::message_create_t &event)
    {
        if (event.msg.content == "<@1469410323776667679>")
            event.reply(":wave: ФСБ is online and functioning!");
    });
}
