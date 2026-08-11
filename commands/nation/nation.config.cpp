#include "nation.hpp"

#include "../../config/enumerations.hpp"
#include "../../utils/database/database.hpp"
#include "../../utils/text/text.hpp"

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

/*
    Edit configuration of a nation.

    Tasks:
        1)

    Parameters:
        - bot       / dpp::cluster              / FSB client data.
        - database  / MYSQL*                    / FSB + MineWorld database.
        - event     / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Nation::nation_config
(
    dpp::cluster                    &bot,
    MYSQL*                          &database,
    const dpp::interaction_create_t &event
)
{
    const std::string user_id = std::to_string(event.command.usr.id);
    Database::Output user_nationality = Database::db_query(database, "SELECT * FROM nationality WHERE user_id = '" + user_id + "' LIMIT 1");

    if (user_nationality.size() == 0)
    {
        event.reply(dpp::message(":warning: You can not perform this action as stateless.").set_flags(dpp::m_ephemeral));
        return;
    }

    const int rank = std::stoi(user_nationality[0]["rank"]);

    if (rank < PRIME_MINISTER)
    {
        event.reply(dpp::message(":warning: You do not have the required permissions to perform this action.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string nation_id = user_nationality[0]["nation_id"];
    Database::Output nations = Database::db_query(database, "SELECT * FROM nations WHERE nation_id = '" + nation_id + "' LIMIT 1");

    if (nations.size() == 0)
    {
        event.reply(dpp::message(":warning: Something went wrong.").set_flags(dpp::m_ephemeral));
        return;
    }

    const std::string government_type = Text::get_government_type(std::stoi(nations[0]["government_type"]));
    const std::string ideology = Text::get_ideology(std::stoi(nations[0]["ideology"]));
    const std::string description = nations[0]["description"];
    const std::string join_condition = Text::get_join_condition(std::stoi(nations[0]["join_condition"]));
    const std::string invite_permission = Text::get_invite_permission(std::stoi(nations[0]["invite_permission"]));

    const dpp::embed embed = dpp::embed()
    .set_color(dpp::colors::light_gray)
    .add_field(":information_source: Information", "**Government**: " + government_type + ".\n**Ideology**: " + ideology + ".")
    .add_field(":question: Description", description)
    .add_field(":envelope_with_arrow: Invitations", "**Join Condition**: " + join_condition + ".\n**Invite Permission**: " + invite_permission + ".");

    const dpp::component buttons = dpp::component()
    .add_component (
        dpp::component()
        .set_label("Government")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_secondary)
        .set_id("government_config")
    )
    .add_component (
        dpp::component()
        .set_label("Ideology")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_secondary)
        .set_id("ideology_config")
    )
    .add_component (
        dpp::component()
        .set_label("Description")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_secondary)
        .set_id("description_config")
    )
    .add_component (
        dpp::component()
        .set_label("Join Condition")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_secondary)
        .set_id("join_condition_config")
    )
    .add_component (
        dpp::component()
        .set_label("Invite Permission")
        .set_type(dpp::cot_button)
        .set_style(dpp::cos_secondary)
        .set_id("invite_permission_config")
    );

    event.reply(dpp::message().add_embed(embed).add_component(buttons).set_flags(dpp::m_ephemeral));
}
