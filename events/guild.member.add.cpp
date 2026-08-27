#include "events.hpp"

#include "../utils/database/database.hpp"
#include "../utils/logs/logs.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Event that triggers when a new member joins a guild.

    Tasks:
        1) Add the listener to the bot.
        2) Handle the new member.
            a. Get some information about the event and log the arrival.
            b. Try to get some configuration from the database.
            c. Try to give the "Member" role to the new member.
            d. Try to send a welcome message.

    Parameters (variable_name / type / description):
        - bot       / dpp::cluster / Client of the bot with all related information.
        - database  / MYSQL*       / Database used for the FSB bot and the MineWorld server.

    Returns (type + description):
        No object returned.
*/
void Events::guild_member_add
(
    dpp::cluster &bot,
    MYSQL*       &database
)
{
    ////////////////// 1) //////////////////
    bot.on_guild_member_add([&bot, &database](const dpp::guild_member_add_t &event)
    {
        ////////////////// 2) //////////////////
        ///////// a. /////////
        const dpp::snowflake guild_id = event.adding_guild.id;
        const dpp::snowflake user_id = event.added.user_id;

        Logs::log("New member on guild " + std::to_string(guild_id) + " -> " + std::to_string(user_id) + ".");

        ///////// b. /////////
        Database::Output config = Database::db_query(database, "SELECT member_role, welcome_channel FROM config LIMIT 1");

        if (config.size() == 0)
        {
            Logs::log("Warning: No config available -> guild member add.");
            return;
        }

        ///////// c. /////////
        const std::string member_role = config[0]["member_role"];
        const bool role_exists = (dpp::find_role(member_role) -> guild_id == guild_id);

        if (role_exists)
            bot.guild_member_add_role(guild_id, user_id, dpp::snowflake(member_role));
        else Logs::log("Warning: member_role ID is not valid -> guild member add.");

        ///////// d. /////////
        const std::string welcome_channel = config[0]["welcome_channel"];
        const bool channel_exists = (dpp::find_channel(welcome_channel) -> guild_id == guild_id);

        if (channel_exists)
            bot.message_create(dpp::message(dpp::snowflake(welcome_channel), ":wave: **Welcome** to <@" + std::to_string(user_id) + "> who **just joined** the server!"));
        else Logs::log("Warning: welcome_channel ID is not valid -> guild member add.");
    });
}
