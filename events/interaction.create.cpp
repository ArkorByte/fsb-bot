#include "events.hpp"

#include "../commands/commands.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>

/*
    Event that triggers when an interaction with FSB is made.

    Tasks:
        1) Add the listener to the bot.
        2) Handle command interactions only for now.

    Parameters:
        - bot      / dpp::cluster / FSB client data.
        - database / MYSQL*       / FSB + MineWorld database.

    Returns:
        No object returned.
*/
void Events::interaction_create
(
    dpp::cluster &bot,
    MYSQL*       &database
)
{
    bot.on_interaction_create([&bot](const dpp::interaction_create_t event)
    {
        if (event.command.get_command_name() == "ban")
            Commands::ban(bot, event);
        else if (event.command.get_command_name() == "journalism")
            Commands::journalism(event);
        else if (event.command.get_command_name() == "kick")
            Commands::kick(bot, event);
        else if (event.command.get_command_name() == "ping")
            Commands::ping(bot, event);
        else if (event.command.get_command_name() == "play")
            Commands::play(event);
        else if (event.command.get_command_name() == "rules")
            Commands::rules(event);
    });
}
