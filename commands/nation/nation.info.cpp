#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/database/database.hpp"
#include "../../utils/text/text.hpp"

#include <dpp/dpp.h>
#include <map>
#include <mysql/mysql.h>
#include <string>

/*
    Display information and stats of a nation.

    Tasks:
        1)
        2)
            a.
            b.
            c.
        3)
            a.
            b.

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::nation_info
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    ////////////////// 1) //////////////////
    std::string nation_id = std::get<std::string>(event.get_parameter("nation_id"));

    ////////////////// 2) //////////////////
    ///////// a. /////////
    nation_id = Database::sanitize_input(database, nation_id);

    ///////// b. /////////
    Database::Output nations = Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":prohibited: Nation `" + nation_id + "` does not exist.").set_flags(dpp::m_ephemeral));
        return;
    }

    ///////// c. /////////
    Database::Output config = Database::db_query(database, "SELECT flags_url FROM config LIMIT 1");

    if (config.size() == 0)
    {
        event.reply(dpp::message(":prohibited: No configuration is available to find required display elements.").set_flags(dpp::m_ephemeral));
        return;
    }

    ////////////////// 3) //////////////////
    ///////// a. /////////
    const std::string display_name = nations[0]["display_name"];
    const std::string description = nations[0]["description"];
    const std::string flag = Text::get_nation_flag(nation_id);

    const std::string government_type = Text::get_government_type(std::stoi(nations[0]["government_type"]));
    const std::string ideology = Text::get_ideology(std::stoi(nations[0]["ideology"]));
    const std::string join_condition = Text::get_join_condition(std::stoi(nations[0]["join_condition"]));
    const std::string role_id = (nations[0]["role_id"] == "0" ? "No role created yet" : "<@&" + nations[0]["role_id"] + "> ||" + nations[0]["role_id"] + "||");

    ///////// b. /////////
    const dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::cream_white)
    .set_title(display_name)
    .set_thumbnail("http://51.75.140.147/flags/" + nation_id + ".png")
    .add_field(":information_source: Description", description)
    .add_field(":eye: Overview", "**Nation ID**: " + nation_id + ".\n**Flag**: " + flag + " ([download from server](http://51.75.140.147/flags/" + nation_id + ".png)).\n**Role**: " + role_id + ".\n**Government**: " + government_type + ".\n**Ideology**: " + ideology + ".\n**Join condition**: " + join_condition + ".");

    const dpp::component select_menu = dpp::component()
    .add_component (
        dpp::component()
        .set_type(dpp::cot_selectmenu)
        .set_id("nation_info")
        .set_placeholder("Select a category of information.")
        .add_select_option(dpp::select_option("Overview", "overview", "Basic information about the nation.").set_emoji(u8"👁️"))
        .add_select_option(dpp::select_option("Politics", "politics", "Political life of the nation.").set_emoji(u8"🗣️"))
        .add_select_option(dpp::select_option("Economy", "economy", "Financial situation of the country.").set_emoji(u8"🪙"))
        .add_select_option(dpp::select_option("Media", "media", "Media and press stats and freedom rating.").set_emoji(u8"📰"))
        .add_select_option(dpp::select_option("Population", "population", "Some information about the people of the nation.").set_emoji(u8"👥"))
        .add_select_option(dpp::select_option("Warfare", "warfare", "Military capacities of the nation.").set_emoji(u8"🪖"))
        .add_select_option(dpp::select_option("United Nations", "united_nations", "International performance of the nation.").set_emoji(u8"🇺🇳"))
    );

    event.reply(dpp::message().add_embed(embed).add_component(select_menu));
}
