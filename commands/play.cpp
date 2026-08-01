#include "commands.hpp"

#include <dpp/dpp.h>

/*
    Gives some information about how to play to MineWorld.

    Tasks:
        1) Reply to the interaction.

    Parameters:
        - event / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Commands::play
(
    const dpp::interaction_create_t &event
)
{
    event.reply
    (
        dpp::message
        (
            std::string("### __How to play to Mineworld__\n") +
            "- Get a **Minecraft Java launcher** (*26.1.2 or higher*).\n" +
            "- Join the server at the **address** `51.75.140.147`.\n" +
            "- **Web interactive map** of the world at `http://51.75.140.147:8100`."
        ).set_flags(dpp::m_ephemeral)
    );
}
