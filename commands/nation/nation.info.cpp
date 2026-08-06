#include "nation.hpp"

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

    Parameters:
        - bot       / dpp::cluster       / FSB client data.
        - database  / MYSQL*             / FSB + MineWorld database.
        - event     / dpp::form_submit_t / Event information.

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
        event.reply(dpp::message(":warning: This nation does not exist!").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string flag = Utils::Text::get_nation_flag(nation_id);
    const std::string display_name = nations[0]["display_name"];
    const std::string description = nations[0]["description"];

    dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::cream_white)
    .set_title(flag + " " + display_name)
    .set_description(description);

    /////////////////////////////
    ///// Field "overview". /////
    /////////////////////////////

    const int join_condition = std::stoi(nations[0]["join_condition"]);
    const std::string role_id = (nations[0]["role_id"] == "0") ? "None" : "<@&" + nations[0]["role_id"] + ">";
    const int government_type = std::stoi(nations[0]["government_type"]);
    const int ideology = std::stoi(nations[0]["ideology"]);
    const std::string nuclear_state = (nations[0]["nuclear_state"] == "0") ? "No" : "Yes";
    const std::string veto_state = (nations[0]["veto_state"] == "0") ? "No" : "Yes";
    const std::string media_blacklist = (nations[0]["media_blacklist"] == "0") ? "Off" : "On";
    const std::string media_whitelist = (nations[0]["media_whitelist"] == "0") ? "Off" : "On";

    embed.add_field(":eye: Overview", "**Nation ID**: " + nation_id + ".\n**Join condition**: " + Utils::Text::get_join_confition(join_condition) + ".\n**Nation role**: " + role_id + ".\n**Government**: " + Utils::Text::get_government_type(government_type) + ".\n**Ideology**: " + Utils::Text::get_ideology(ideology) + ".\n**Nuclear state**: " + nuclear_state + ".\n**Veto state**: " + veto_state + ".\n**Media blacklist**: " + media_blacklist + ".\n**Media whitelist**: " + media_whitelist + ".");

    ////////////////////////////
    ///// Field "members". /////
    ////////////////////////////

    Utils::Database::QueryData leader_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE nation_id = '" + nation_id + "' AND rank = 3 LIMIT 1");
    Utils::Database::QueryData pm_nationality = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE nation_id = '" + nation_id + "' AND rank = 2 LIMIT 1");
    Utils::Database::QueryData government = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE nation_id = '" + nation_id + "' AND RANK = 1");
    Utils::Database::QueryData citizens = Utils::Database::db_query(database, "SELECT * FROM nationality WHERE nation_id = '" + nation_id + "' AND RANK = 0");

    const std::string leader = (leader_nationality.size() != 0) ? "<@" + leader_nationality[0]["user_id"] + ">" : "None";
    const std::string prime_minister = (pm_nationality.size() != 0) ? "<@" + pm_nationality[0]["user_id"] + ">" : "None";

    std::string government_list;
    std::string citizens_list;

    if (government.size() != 0)
    {
        for (std::map<std::string, std::string> &user : government)
        {
            if (!government_list.empty())
                government_list += ", ";

            government_list += "<@" + user["user_id"] + ">";
        }
    }
    else government_list = "None";

    if (citizens.size() != 0)
    {
        for (std::map<std::string, std::string> &user : citizens)
        {
            if (!citizens_list.empty())
                citizens_list += ", ";

            citizens_list += "<@" + user["user_id"] + ">";
        }
    }
    else citizens_list = "None";

    embed.add_field(":bust_in_silhouette: Members", "**Head of State**: " + leader + ".\n**Prime minister**: " + prime_minister + ".\n**Government**: " + government_list + ".\n**Citizens**: " + citizens_list + ".");

    //////////////////////////////
    ///// Field "statistics" /////
    //////////////////////////////

    const std::string tux_balance = nations[0]["tux_balance"];
    const std::string media_freedom = nations[0]["media_freedom"];
    const std::string media_freedom_rating = Utils::Text::get_media_freedom_rating(std::stoi(media_freedom));
    const std::string media_posts = nations[0]["media_posts"];
    const std::string censored_posts = nations[0]["censored_posts"];
    const std::string leadership_changes = nations[0]["leadership_changes"];
    const std::string government_changes = nations[0]["government_changes"];
    const std::string veto_usage_count = nations[0]["veto_usage_count"];
    const std::string resolutions_count = nations[0]["resolutions_count"];
    const std::string passed_resolutions = nations[0]["passed_resolutions"];

    embed.add_field(":bar_chart: Statistics", "**Tux balance**: " + tux_balance + ".\n**Media freedom**: " + media_freedom_rating + " (" + media_freedom + "%).\n**Media posts**: " + media_posts + ".\n**Censored posts**: " + censored_posts + ".\n**Leadership changes**: " + leadership_changes + ".\n**Government changes**: " + government_changes + ".\n**Used vetoes**: " + veto_usage_count + ".\n**Resolutions count**: " + resolutions_count + ".\n**Passed resolutions**: " + passed_resolutions + ".");

    /////////////////////////
    ///// Field "time". /////
    /////////////////////////

    const std::string claim_time = (nations[0]["claim_time"] == "0") ? "Never" : "<t:" + nations[0]["claim_time"] + ":f>";
    const std::string last_post = (nations[0]["last_post"] == "0") ? "Never" : "<t:" + nations[0]["last_post"] + ":f>";
    const std::string last_manual_censorship = (nations[0]["last_manual_censorship"] == "0") ? "Never" : "<t:" + nations[0]["last_manual_censorship"] + ":f>";
    const std::string last_automatic_censorship = (nations[0]["last_automatic_censorship"] == "0") ? "Never" : "<t:" + nations[0]["last_automatic_censorship"] + ":f>";
    const std::string last_leadership_change = (nations[0]["last_leadership_change"] == "0") ? "Never" : "<t:" + nations[0]["last_leadership_change"] + ":f>";
    const std::string last_government_change = (nations[0]["last_government_change"] == "0") ? "Never" : "<t:" + nations[0]["last_government_change"] + ":f>";
    const std::string acquired_nuclear_time = (nations[0]["acquired_nuclear_time"] == "0") ? "Never" : "<t:" + nations[0]["acquired_nuclear_time"] + ":f>";
    const std::string last_veto_usage = (nations[0]["last_veto_usage"] == "0") ? "Never" : "<t:" + nations[0]["last_veto_usage"] + ":f>";
    const std::string last_resolution = (nations[0]["last_resolution"] == "0") ? "Never" : "<t:" + nations[0]["last_resolution"] + ":f>";
    const std::string last_passed_resolution = (nations[0]["last_passed_resolution"] == "0") ? "Never" : "<t:" + nations[0]["last_passed_resolution"] + ":f>";

    embed.add_field(":hourglass: Time", "**Claim time**: " + claim_time + ".\n**Last post**: " + last_post + ".\n**Last manual censorship**: " + last_manual_censorship + ".\n**Last automatic censorship**: " + last_automatic_censorship + ".\n**Last leadership change**: " + last_leadership_change + ".\n**Last government change**: " + last_government_change + ".\n**Acquired nuclear**: " + acquired_nuclear_time + ".\n**Last veto**: " + last_veto_usage + ".\n**Last resolution**: " + last_resolution + ".\n**Last passed resolution**: " + last_passed_resolution + ".");

    event.reply(dpp::message().add_embed(embed).set_flags(dpp::m_ephemeral));
}
