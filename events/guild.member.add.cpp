#include "events.hpp"

#include "../utils/utils.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Event that triggers when a new member joins the server.

    Tasks:
        1) Log new arrival.
        2) Retrieve server config from database.
        3) Get all required roles and channel IDs.
        4) Attribute new roles if they are valid.
        5) Send a welcome message if the channel configured is valid.

    Parameters:
        - bot      / dpp::cluster / FSB client data.
        - database / MYSQL*       / FSB + MineWorld database.

    Returns:
        No object returned.
*/
void Events::guild_member_add
(
    dpp::cluster &bot,
    MYSQL*       &database
)
{
    bot.on_guild_member_add([&bot, &database](const dpp::guild_member_add_t &event)
    {
        const int64_t guild_id = event.adding_guild.id;
        const int64_t user_id = event.added.user_id;

        Utils::Logs::log("New member on guild " + std::to_string(guild_id) + " -> " + std::to_string(user_id) + ".");
        Utils::Database::QueryData config = Utils::Database::db_query(database, "SELECT * FROM config WHERE guild_id = " + std::to_string(guild_id) + " LIMIT 1");

        if (config.size() < 1)
        {
            Utils::Logs::log("CONFIG NOT AVAILABLE FOR SERVER " + std::to_string(guild_id) + "! TODO: auto fix this error.");
            return;
        }

        const int64_t member_role = std::get<int64_t>(config[0]["member_role"]);
        const int64_t stateless_role = std::get<int64_t>(config[0]["stateless_role"]);
        const int64_t welcome_channel = std::get<int64_t>(config[0]["welcome_channel"]);

        if (dpp::find_role(member_role) -> guild_id == guild_id)
            bot.guild_member_add_role(guild_id, user_id, member_role);
        else Utils::Logs::log("FIX NEEDED, the member_role ID is not valid!");

        if (dpp::find_role(stateless_role) -> guild_id == guild_id)
            bot.guild_member_add_role(guild_id, user_id, stateless_role);
        else Utils::Logs::log("FIX NEEDED, the stateless_role ID is not valid!");

        if (dpp::find_channel(welcome_channel) -> guild_id == guild_id)
            bot.message_create(dpp::message(welcome_channel, ":wave: **Welcome** to <@" + std::to_string(user_id) + "> who **just joined** the server!"));
        else Utils::Logs::log("FIX NEEDED, the welcome_channel ID is not valid!");
    });
}
