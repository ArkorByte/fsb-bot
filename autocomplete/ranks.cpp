#include "autocomplete.hpp"

#include <dpp/dpp.h>

/*
    Auto complete slash commands that need to display all available ranks.

    Tasks:
        1) Verify that it is the correct parameter being focused.
        2) Display all possible ranks at all time.

    Parameters:
        - bot      / dpp::cluster       / FSB client data.
        - event    / dpp::form_submit_t / Event information.

    Returns:
        No object returned.
*/
void Autocomplete::ranks
(
    dpp::cluster              &bot,
    const dpp::autocomplete_t &event
)
{
    for (const auto &option : event.options)
        if (option.focused && option.name != "new_rank") return;

    bot.interaction_response_create
    (
        event.command.id,
        event.command.token,
        dpp::interaction_response(dpp::ir_autocomplete_reply)
            .add_autocomplete_choice(dpp::command_option_choice("Citizen", "0"))
            .add_autocomplete_choice(dpp::command_option_choice("Military", "1"))
            .add_autocomplete_choice(dpp::command_option_choice("Minister", "2"))
            .add_autocomplete_choice(dpp::command_option_choice("Prime Minister", "3"))
            .add_autocomplete_choice(dpp::command_option_choice("Head of State", "4"))
    );
}
