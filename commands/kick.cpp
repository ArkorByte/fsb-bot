#include "commands.hpp"

#include "../utils/utils.hpp"

#include <dpp/dpp.h>
#include <string>

/*
    Kick a member from the Discord server.

    Tasks:
        1) Get command parameters.
        2) Verify that the user is not kicking themselves.
        3) Verify that FSB is not targeted.
        4) Try to retrieve the member using provided user ID.
        5) Try to retrieve guild data.
        6) Verify that the guild owner is not targeted.
        7) Get command's executor data and their permissions.
        8) Verify that the executor has the dpp::p_kick_members permission.
        9) Get target and executor highest role positions to verify if the kick would normally be possible.
        10) Get bot's permissions and verify that FSB has the dpp::p_kick_members permission.
        11) Verify that FSB roles are high enough to be able to kick the member.
        12) Set the reason.
        13) Try to kick the member from the server.

    Parameters:
        - bot   / dpp::cluster              / FSB client data.
        - event / dpp::interaction_create_t / Event information.

    Returns:
        No object returned.
*/
void Commands::kick
(
    dpp::cluster                    &bot,
    const dpp::interaction_create_t &event
)
{
    const dpp::snowflake user_id = std::get<dpp::snowflake>(event.get_parameter("member"));
    const std::string reason = std::get<std::string>(event.get_parameter("reason"));
    const bool silent = std::get<bool>(event.get_parameter("silent"));

    const dpp::snowflake executer_id = event.command.usr.id;

    if (user_id == executer_id)
    {
        event.reply(dpp::message(":warning: You are not allowed to kick yourself!").set_flags(dpp::m_ephemeral));
        return;
    }

    if (user_id == bot.me.id)
    {
        event.reply(dpp::message("<:putin_gun:1516736231357153491> ФСБ WILL NOT go down without a fight, сука!").set_flags(dpp::m_ephemeral));
        return;
    }

    const auto member_it = event.command.resolved.members.find(user_id);

    if (member_it == event.command.resolved.members.end())
    {
        event.reply(dpp::message(":warning: This member was not found on this server!").set_flags(dpp::m_ephemeral));
        return;
    }

    const dpp::snowflake guild_id = event.command.guild_id;
    const dpp::guild *guild = dpp::find_guild(guild_id);

    if (!guild)
    {
        event.reply(dpp::message(":warning: Something went wrong while retrieving server data!").set_flags(dpp::m_ephemeral));
        return;
    }

    if (user_id == guild -> owner_id)
    {
        event.reply(dpp::message(":warning: You can not kick the server owner!").set_flags(dpp::m_ephemeral));
        return;
    }

    const dpp::guild_member executer = event.command.member;
    const dpp::permission executer_permissions = guild -> base_permissions(executer);

    if (!(executer_permissions & dpp::p_kick_members))
    {
        event.reply(dpp::message(":warning: You do not have the required permissions to execute this command!").set_flags(dpp::m_ephemeral));
        return;
    }

    const dpp::guild_member member = member_it -> second;
    const dpp::permission member_permissions = guild -> base_permissions(member);

    if ((member_permissions & dpp::p_administrator) && executer_id != guild -> owner_id)
    {
        event.reply(dpp::message(":warning: Only the server owner is allowed to kick administrators!").set_flags(dpp::m_ephemeral));
        return;
    }

    const uint8_t highest_member_role = Utils::Miscellaneous::highest_role_position(member);
    const uint8_t highest_executer_role = Utils::Miscellaneous::highest_role_position(executer);

    if (highest_member_role >= highest_executer_role && executer_id != guild -> owner_id)
    {
        event.reply(dpp::message(":warning: You can not kick this member.").set_flags(dpp::m_ephemeral));
        return;
    }

    const dpp::permission bot_permissions = event.command.app_permissions;

    if (!(bot_permissions & dpp::p_kick_members))
    {
        event.reply(dpp::message(":warning: ФСБ does not have the required permissions to kick!").set_flags(dpp::m_ephemeral));
        return;
    }

    const auto bot_member_it = guild -> members.find(bot.me.id);

    if (bot_member_it == guild -> members.end())
    {
        event.reply(dpp::message(":warning: Failed to find ФСБ in the guild members list!").set_flags(dpp::m_ephemeral));
        return;
    }

    const dpp::guild_member bot_member = bot_member_it -> second;
    const uint8_t highest_bot_role = Utils::Miscellaneous::highest_role_position(bot_member);

    if (highest_member_role >= highest_bot_role)
    {
        event.reply(dpp::message(":warning: ФСБ can not kick this member!").set_flags(dpp::m_ephemeral));
        return;
    }

    bot.set_audit_reason("[" + std::to_string(executer_id) + "] " + reason);

    bot.guild_member_kick(guild_id, user_id, [=](const dpp::confirmation_callback_t &callback)
    {
        if (callback.is_error())
        {
            event.reply(dpp::message(":warning: Something went wrong while kicking the member!").set_flags(dpp::m_ephemeral));
            return;
        }

        event.reply(dpp::message(":white_check_mark: <@" + std::to_string(user_id) + "> has been successfully kicked!").set_flags(silent ? dpp::m_ephemeral : 0));
        Utils::Logs::log("[ban] " + std::to_string(executer_id) + " kicked " + std::to_string(user_id) + " from " + std::to_string(guild_id) +  " for \"" + reason + "\"!");
    });
}
