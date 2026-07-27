#include "commands.hpp"

#include <dpp/dpp.h>

/*
    Display a modal.

    Tasks:
        1)

    Parameters:
        - event / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Commands::journalism
(
    const dpp::interaction_create_t &event
)
{
    dpp::interaction_modal_response modal("journalism_modal", "Create new article");

    modal.add_component
    (
        dpp::component()
        .set_id("title")
        .set_label("Article title")
        .set_type(dpp::cot_text)
        .set_placeholder("Enter your article title here.")
        .set_min_length(3)
        .set_max_length(50)
        .set_text_style(dpp::text_short)
        .set_required(true)
    );

    modal.add_component
    (
        dpp::component()
        .set_id("content")
        .set_label("Article content")
        .set_type(dpp::cot_text)
        .set_placeholder("Write your article here.")
        .set_min_length(20)
        .set_max_length(4000)
        .set_text_style(dpp::text_paragraph)
        .set_required(true)
    );

    modal.add_component
    (
        dpp::component()
        .set_id("top_right_image")
        .set_label("Top right image (optional)")
        .set_type(dpp::cot_text)
        .set_placeholder("Enter image URL here.")
        .set_min_length(1)
        .set_max_length(1000)
        .set_text_style(dpp::text_short)
    );

    modal.add_component
    (
        dpp::component()
        .set_id("bottom_image")
        .set_label("Bottom image (optional)")
        .set_type(dpp::cot_text)
        .set_placeholder("Enter image URL here.")
        .set_min_length(1)
        .set_max_length(1000)
        .set_text_style(dpp::text_short)
    );

    event.dialog(modal);
}
