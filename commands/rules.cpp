#include "commands.hpp"

#include <dpp/dpp.h>

/*
    Gives server rules to the user.

    Tasks:
        1) Reply to the interaction.

    Parameters:
        - event / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Commands::rules
(
    const dpp::interaction_create_t &event
)
{
    event.reply
    (
        dpp::message
        (
            std::string("## :judge: Rules\n") +
            ":warning: The staff and owners of this project are **NOT responsible** in case of losses, doxxing, scams, harassment or **ANY harmful actions** committed by **non-staff members**.\n" +
            ":flag_fr: The project complies with **French laws** due to servers localization.\n" +
            ":police_car: The staff can **sanction** and **censor** members at their **own discretion**. FSB can also **automatically censor** content.\n\n" +
            "### :speech_left: Discord server\n" +
            "- You must comply with **Discord Terms of Service** and **Community Guidelines** that you may find [here](https://discord.com/terms).\n" +
            "- **Impersonating** staff members is **STRICTLY PROHIBITED**.\n" +
            "- Talk in the **right channels**, off-topic conversations may be **deleted**.\n" +
            "- Due to **moderation concerns**, any message written in any language that is not **English**, **French** or **Russian** may be removed (*this list depends on staff proficiency in certain languages and may be updated*).\n"
            "- Using `@everyone` and `@here` are **reserved** to **staff ONLY**.\n" +
            "- Otherwise, anything that is not **EXPLICITLY marked** as prohibited is **IMPLICITLY allowed**.\n\n" +
            "### :pick: MineWorld\n" +
            "- Using **cheats**, **hacked clients** or **custom mods** modifying the behavior of your game and/or giving gameplay advantages are **STRICTLY PROHIBITED**.\n" +
            "- **Optimization** and **texture pack mods** such as OptiFine, Embeddium or shaders are **TOLERATED**.\n" +
            "- In-game **glitches** and **exploits** are NOT prohibited but **may be patched** by the development team at their own discretion.\n" +
            "- Purposefully making the server **lag**, **crash** and **disrupting stability** or **attempts of piracy** are **STRICTLY PROHIBITED**.\n" +
            "- The usage of **automated clients** and **bots** are **PROHIBITED**.\n" +
            "- Otherwise, anything that is not **EXPLICITLY marked** as prohibited is **IMPLICITLY allowed**."
        ).set_flags(dpp::m_ephemeral)
    );
}
