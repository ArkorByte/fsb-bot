#include "miscellaneous.hpp"

#include <dpp/dpp.h>
#include <vector>

/*
    Return the position of the highest role of a guild member.

    Tasks:
        1) Setup the starting highest position to 0 (no role).
        2) Analyze the member roles.
            a. Check each role position by ID.
            b. If it's the highest role found yet, we register it as it.

    Parameters (variable_name / type / description):
        - member / dpp::guild_member / Targeted member of the guild.

    Returns (type + description):
        A uint8_t containing the position index of the highest role of this member.
*/
uint8_t Miscellaneous::highest_role_position
(
    const dpp::guild_member &member
)
{
    ////////////////// 1) //////////////////
    uint8_t highest_position = 0;

    ////////////////// 2) //////////////////
    ///////// a. /////////
    for (const dpp::snowflake &role_id : member.get_roles())
    {
        const dpp::role *role = dpp::find_role(role_id);
        const uint8_t position = role -> position;

        ///////// b. /////////
        if (role && position > highest_position)
            highest_position = position;
    }

    return highest_position;
}
