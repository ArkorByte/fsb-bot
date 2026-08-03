#include "modals.hpp"

#include "../config/nations.hpp"
#include "../utils/utils.hpp"

#include <algorithm>
#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Display a post in journalism

    Tasks:
        1) Get modal fields data.
        2) Get user nationality.
        3) Get nation information.
        4) Get whitelist status and related stats.
        5) If whitelist is enabled and user not in it, we automatically censor and update stats.
        6) Get blacklist status.
        7) If blacklist enabled and user in it, we automatically censor and update stats / media freedom rating.
        8) Otherwise, we update media freedom depending on whitelist status and display the post in journalism channel.

    Parameters:
        - bot      / dpp::cluster       / FSB client data.
        - database / MYSQL*             / FSB + MineWorld database.
        - event    / dpp::form_submit_t / Event information.

    Returns:
        No object returned.
*/
void Modals::journalism
(
    dpp::cluster             &bot,
    MYSQL*                   &database,
    const dpp::form_submit_t &event
)
{
    const std::string article_title = std::get<std::string>(event.components[0].value);
    const std::string article_content = std::get<std::string>(event.components[1].value);
    const std::string top_image_url = std::get<std::string>(event.components[2].value);
    const std::string bottom_image_url = std::get<std::string>(event.components[3].value);

    const int64_t user_id = event.command.usr.id;
    Utils::Database::QueryData nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + std::to_string(user_id) + "' LIMIT 1");

    if (nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not post anything as stateless. Join a nation first.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string nation_id = nationality[0]["nation_id"];
    Utils::Database::QueryData nation = Utils::Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nation.size() == 0)
    {
        event.reply(dpp::message(":warning: Something went **wrong**!").set_flags(dpp::m_ephemeral));
        Utils::Logs::log("FIX NEEDED: Nation ID " + nation_id + " missing in database.");
        return;
    }

    const std::string whitelist = nation[0]["media_whitelist"];
    const int64_t media_posts = std::stoll(nation[0]["media_posts"]) + 1;
    const int64_t timestamp = Utils::Miscellaneous::get_current_timestamp();

    Utils::Database::QueryData journalism_status = Utils::Database::db_query(database, "SELECT * FROM journalism WHERE user_id = '" + std::to_string(user_id) + "' LIMIT 1");
    int user_status = -1;

    if (journalism_status.size() != 0)
        user_status = std::stoi(journalism_status[0]["status"]);

    if (whitelist == "1" && user_status != 0)
    {
        event.reply(dpp::message(":warning: Your post has been **automatically censored** by your government due to the **highest censorship** measures in place!").set_flags(dpp::m_ephemeral));
        const int64_t censored_posts = std::stoll(nation[0]["censored_posts"]) + 1;

        Utils::Database::db_query(database, "UPDATE nations SET media_posts = '" + std::to_string(media_posts) + "', censored_posts = '" + std::to_string(censored_posts) + "', last_a_censorship = '" + std::to_string(timestamp) + "' WHERE nation_id = '" + nation_id + "'");
        return;
    }

    const std::string blacklist = nation[0]["media_blacklist"];

    if (blacklist == "1" && user_status == 1)
    {
        event.reply(dpp::message(":warning: Your post has been **automatically censored** by your government due to **censorship restrictions** against you!").set_flags(dpp::m_ephemeral));

        const int64_t media_freedom = std::clamp(std::stoll(nation[0]["media_freedom"]) - 1, 0LL, 100LL);
        const int64_t censored_posts = std::stoll(nation[0]["censored_posts"]) + 1;

        Utils::Database::db_query(database, "UPDATE nations SET media_freedom = '" + std::to_string(media_freedom) + "', media_posts = '" + std::to_string(media_posts) + "', censored_posts = '" + std::to_string(censored_posts) + "', last_a_censorship = '" + std::to_string(timestamp) + "' WHERE nation_id = '" + nation_id + "'");
        return;
    }

    const int change = whitelist == "1" ? 0 : 1;
    const int64_t media_freedom = std::clamp(std::stoll(nation[0]["media_freedom"]) + change, 0LL, 100LL);
    const std::string nation_name = nation[0]["display_name"];
    const std::string flag = nation_flags[nation_id];

    Utils::Database::db_query(database, "UPDATE nations SET media_freedom = '" + std::to_string(media_freedom) + "', media_posts = '" + std::to_string(media_posts) + "', last_post = '" + std::to_string(timestamp) + "' WHERE nation_id = '" + nation_id + "'");

    const dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::cream_white)
    .set_title(flag + " " + article_title)
    .set_description(article_content)
    .set_thumbnail(top_image_url)
    .set_image(bottom_image_url)
    .set_footer(dpp::embed_footer().set_text("Published by " + event.command.usr.username + " from " + flag + " " + nation_name + "."));

    const dpp::component buttons = dpp::component()
    .add_component (
        dpp::component()
        .set_label("Censor")
        .set_emoji(u8"🤫")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_premium)
        .set_id("journalism_censor")
    )
    .add_component (
        dpp::component()
        .set_label("Blacklist")
        .set_emoji(u8"⚠️")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_danger)
        .set_id("journalism_blacklist")
    );

    const int64_t guild_id = event.command.guild_id;

    Utils::Database::QueryData config = Utils::Database::db_query(database, "SELECT * FROM config WHERE guild_id = '" + std::to_string(guild_id) + "' LIMIT 1");
    const int64_t journalism_channel = std::stoll(config[0]["journalism_channel"]);

    if (dpp::find_channel(journalism_channel) -> guild_id == guild_id)
    {
        bot.message_create(dpp::message(journalism_channel, "||" + std::to_string(user_id) + "." + nation_id + "||").add_embed(embed).add_component(buttons));
        event.reply(dpp::message(":white_check_mark: Your post has been punished!").set_flags(dpp::m_ephemeral));
    }
    else
    {
        Utils::Logs::log("FIX NEEDED, journalism_channel ID is not valid!");
        event.reply(dpp::message(":warning: Something went wrong finding the journalism channel!").set_flags(dpp::m_ephemeral));
    };
}
