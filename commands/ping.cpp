#include "commands.hpp"

#include "../utils/utils.hpp"

#include <dpp/dpp.h>
#include <string>

/*
    Get the latency of the bot.

    Tasks:
        1) Get discord client with first shard.
        2) Get WS latency and multiply it by 1000 get ms since dpp returns seconds.

    Parameters:
        - bot   / dpp::cluster              / FSB client data.
        - event / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Commands::ping
(
    const dpp::cluster              &bot,
    const dpp::interaction_create_t &event
)
{
    const dpp::discord_client *shard = bot.get_shard(0);

    if (!shard)
    {
        event.reply(dpp::message(":warning: Something went wrong while getting shard.").set_flags(dpp::m_ephemeral));
        return;
    }

    const double latency = shard -> websocket_ping * 1000;

    if (latency <= 0)
    {
        event.reply(dpp::message(":warning: Gateway API is still warming up..").set_flags(dpp::m_ephemeral));
        return;
    }

    event.reply(dpp::message(":ping_pong: ФСБ latency: " + std::to_string((int) latency) + "ms.").set_flags(dpp::m_ephemeral));
}
