#include "buttons.hpp"

#include "../utils/utils.hpp"

#include <algorithm>
#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Censor a post made in the journalism channel.

    Tasks:
        1) Verify that the executer is part of any nation.
        2) Verify that the executer is part of the nation government.
        3) Get information about the user who made the post and what was his nationality at the time of posting.
        4) Verify that the two nationalities match.
        5) If the user who posted is still in the same nation, we compare the ranks.
        6) Update stats, suppress embed to get rid of the post and set the message content to censored by government.
        7) If the button pressed was journalism_blacklist, we set the user journalism status as blacklisted.

    Parameters:
        - bot      / dpp::cluster       / FSB client data.
        - database / MYSQL*             / FSB + MineWorld database.
        - event    / dpp::form_submit_t / Event information.

    Returns:
        No object returned.
*/
void Buttons::journalism_censor_button
(
    dpp::cluster        &bot,
    const std::string   &custom_id,
    MYSQL*              &database,
    dpp::button_click_t &event
)
{
    const int64_t executer_id = event.command.usr.id;
    Utils::Database::QueryData executer_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + std::to_string(executer_id) + "' LIMIT 1");

    if (executer_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You are not allowed to censor a post from another nation!").set_flags(dpp::m_ephemeral));
        return;
    }

    const int executer_rank = std::stoi(executer_nationality[0]["rank"]);

    if (executer_rank == 0)
    {
        event.reply(dpp::message(":warning: You are not allowed to perform this action!").set_flags(dpp::m_ephemeral));
        return;
    }

    const int message_length = event.command.msg.content.size();
    const std::string message_content = event.command.msg.content.substr(2, message_length - 4);
    const size_t dot_position = message_content.find(".");

    if (dot_position == std::string::npos)
    {
        event.reply(dpp::message(":warning: Something went wrong!").set_flags(dpp::m_ephemeral));
        return;
    }

    const int64_t user_id = std::stoll(message_content.substr(0, dot_position));
    const std::string message_nation_id = message_content.substr(dot_position + 1);
    const std::string executer_nation_id = executer_nationality[0]["nation_id"];

    if (executer_nation_id != message_nation_id)
    {
        event.reply(dpp::message(":warning: You are not allowed to censor a post from another nation!").set_flags(dpp::m_ephemeral));
        return;
    }

    Utils::Database::QueryData user_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + std::to_string(user_id) + "' LIMIT 1");

    if (user_nationality.size() != 0)
    {
        const std::string user_nation_id = user_nationality[0]["nation_id"];
        const int user_rank = std::stoi(user_nationality[0]["rank"]);

        if (user_nation_id == executer_nation_id && user_rank >= executer_rank)
        {
            event.reply(dpp::message(":warning: You are not allowed to censor other government officials that have the same rank or a rank higher than yours!").set_flags(dpp::m_ephemeral));
            return;
        }
    }

    Utils::Database::QueryData nation = Utils::Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + executer_nation_id + "' LIMIT 1");

    if (nation.size() == 0)
    {
        event.reply(dpp::message(":warning: Something went wrong!").set_flags(dpp::m_ephemeral));
        Utils::Logs::log("FIX NEEDED: Nation ID " + executer_nation_id + " missing in database.");
        return;
    }

    const int score_hit = custom_id == "journalism_blacklist" ? 8 : 3;
    const int media_freedom = std::clamp(std::stoll(nation[0]["media_freedom"]) - score_hit, 0LL, 100LL);
    const int censored_posts = std::stoll(nation[0]["censored_posts"]) + 1;
    const std::string timestamp = std::to_string(Utils::Miscellaneous::get_current_timestamp());

    Utils::Database::db_query(database, "UPDATE nations SET media_freedom = '" + std::to_string(media_freedom) + "', censored_posts = '" + std::to_string(censored_posts) + "', last_manual_censorship = '" + timestamp + "' WHERE nation_id = '" + executer_nation_id + "'");

    dpp::message message = event.command.msg;
    const std::string display_name = nation[0]["display_name"];

    message.suppress_embeds();
    message.set_content(":warning: Post taken down by the government of " + display_name + ".");
    bot.message_edit(message);

    if (custom_id == "journalism_blacklist")
    {
        Utils::Database::QueryData user_registered = Utils::Database::db_query(database, "SELECT * FROM journalism WHERE user_id = '" + std::to_string(user_id) + "' LIMIT 1");

        if (user_registered.size() == 0)
            Utils::Database::db_query(database, "INSERT INTO journalism (user_id, status) VALUES ('" + std::to_string(user_id) + "', 1)");
        else Utils::Database::db_query(database, "UPDATE journalism SET status = 1 WHERE user_id = '" + std::to_string(user_id) + "'");

        event.reply(dpp::message(":white_check_mark: This post has been censored and the user blacklisted! Your media freedom rating was hit by 8 points (5 + 3)."));
    }
    else event.reply(dpp::message(":white_check_mark: This post has been censored successfully! Your media freedom rating was hit by 3 points.").set_flags(dpp::m_ephemeral));
}
