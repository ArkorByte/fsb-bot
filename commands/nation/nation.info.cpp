#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/utils.hpp"

#include <dpp/dpp.h>
#include <map>
#include <mysql/mysql.h>
#include <string>

/*
    Display information and stats of a nation.

    Tasks:
        1) Verify nation ID.
        2) Retrieve information from db and build embed.
        3) Build select menu.

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
    std::string nation_id = std::get<std::string>(event.get_parameter("nation_id"));

    nation_id = Utils::Database::sanitize_input(database, nation_id);
    Utils::Database::QueryData nations = Utils::Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":warning: Nation `" + nation_id + "` does not exist.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string display_name = nations[0]["display_name"];
    const std::string description = nations[0]["description"];
    const std::string flag = Utils::Text::get_nation_flag(nation_id);

    const std::string claim_time = (nations[0]["claim_time"] == "0") ? "Never" : "<t:" + nations[0]["claim_time"] + ":f>";
    const std::string government_type = Utils::Text::get_government_type(std::stoi(nations[0]["government_type"]));
    const std::string ideology = Utils::Text::get_ideology(std::stoi(nations[0]["ideology"]));
    const std::string invite_permission = Utils::Text::get_invite_permission(std::stoi(nations[0]["invite_permission"]));
    const std::string join_condition = Utils::Text::get_join_condition(std::stoi(nations[0]["join_condition"]));
    const std::string role_id = "<@&" + nations[0]["role_id"] + "> ||" + nations[0]["role_id"] + "||";

    const dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::cream_white)
    .set_title(display_name)
    .set_thumbnail("http://51.75.140.147/flags/" + nation_id + ".png")
    .add_field(":information_source: Description", description)
    .add_field(":eye: Overview", "**Nation ID**: " + nation_id + ".\n**Flag**: " + flag + " [download from server here](http://51.75.140.147/flags/" + nation_id + ".png).\n**Role**: " + role_id + ".\n**Government**: " + government_type + ".\n**Ideology**: " + ideology + ".\n**Join condition**: " + join_condition + ".\n**Invite Permission**: " + invite_permission + ".");

    const dpp::component select_menu = dpp::component()
    .add_component (
        dpp::component()
        .set_type(dpp::cot_selectmenu)
        .set_id("nation_info")
        .set_placeholder("Select a category of information.")
        .add_select_option(dpp::select_option("Overview", "overview", "Display basic information about the nation.").set_emoji(u8"👁️"))
        .add_select_option(dpp::select_option("Politics", "politics", "Display political information about the nation.").set_emoji(u8"🗣️"))
        .add_select_option(dpp::select_option("Media", "media", "Display media/press information related to the nation.").set_emoji(u8"📰"))
        .add_select_option(dpp::select_option("United Nations", "united_nations", "Display United Nations information related to the nation.").set_emoji(u8"🇺🇳"))
        .add_select_option(dpp::select_option("Economy", "economy", "Display economics related to the nation.").set_emoji(u8"🪙"))
        .add_select_option(dpp::select_option("Population", "population", "Display population stats of the nation.").set_emoji(u8"👥"))
        .add_select_option(dpp::select_option("Warfare", "warfare", "Display military information of the nation.").set_emoji(u8"🪖"))
    );

    event.reply(dpp::message().add_embed(embed).add_component(select_menu));
}
