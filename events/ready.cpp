#include "events.hpp"

#include "../utils/utils.hpp"

#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <string>
#include <vector>

/*
    Code ran on bot startup.

    Tasks:
        1) Enable D++ logs.
        2) Deploy slash commands.
        3) Set bot status with a timer on Discord.

    Parameters:
        - bot / dpp::cluster / FSB client data.

    Returns:
        No object returned.
*/
void Events::ready
(
    dpp::cluster &bot
)
{
    Utils::Logs::log("Booting up ФСБ and deploying slash commands..");

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot](const dpp::ready_t &event)
    {
        if (dpp::run_once<struct register_bot_commands>())
        {
            std::vector<dpp::slashcommand> slash_commands;

            // Command /alliance.
            dpp::slashcommand alliance_command("alliance", "Commands related to alliances.", bot.me.id);

                // Subcommand /alliance config.
                dpp::command_option alliance_config(dpp::co_sub_command, "config", "Edit the configuration of an alliance");
                alliance_command.add_option(alliance_config);

                // Subcommand /alliance create.
                dpp::command_option alliance_create(dpp::co_sub_command, "create", "Create a new alliance.");
                alliance_command.add_option(alliance_create);

                // Subcommand /alliance info.
                dpp::command_option alliance_info(dpp::co_sub_command, "info", "Get information about an alliance.");
                alliance_command.add_option(alliance_info);

                // Subcommand /alliance invite.
                dpp::command_option alliance_invite(dpp::co_sub_command, "invite", "Invite a nation into an alliance.");
                alliance_command.add_option(alliance_invite);

                // Subcommand /alliance join.
                dpp::command_option alliance_join(dpp::co_sub_command, "join", "Join an alliance.");
                alliance_command.add_option(alliance_join);

                // Subcommand /alliance kick.
                dpp::command_option alliance_kick(dpp::co_sub_command, "kick", "Kick a nation from an alliance.");
                alliance_command.add_option(alliance_kick);

                // Subcommand /alliance list.
                dpp::command_option alliance_list(dpp::co_sub_command, "list", "Display a list with all alliances of the server.");
                alliance_command.add_option(alliance_list);

                // Subcommand /alliance rank.
                dpp::command_option alliance_rank(dpp::co_sub_command, "rank", "Change the rank of a player within your nation.");
                alliance_command.add_option(alliance_rank);

            slash_commands.push_back(alliance_command);

            // Command /ban.
            dpp::slashcommand ban_command("ban", "Permanently deport a member from the Discord server.", bot.me.id);
	        ban_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to deport.", true));
            ban_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of the deportation.", true));
            slash_commands.push_back(ban_command);

            // Command /config.
            slash_commands.push_back(dpp::slashcommand("config", "Edit bot configuration.", bot.me.id));

            // Command /nation.
            dpp::slashcommand nation_command("nation", "Commands related to factions.", bot.me.id);

                // Subcommand /nation config.
                dpp::command_option nation_config(dpp::co_sub_command, "config", "Edit the configuration of your nation.");
                nation_command.add_option(nation_config);

                // Subcommand /nation create.
                dpp::command_option nation_create(dpp::co_sub_command, "create", "Create a new nation.");
                nation_command.add_option(nation_create);

                // Subcommand /nation info.
                dpp::command_option nation_info(dpp::co_sub_command, "info", "Get information about a nation.");
                nation_command.add_option(nation_info);

                // Subcommand /nation invite.
                dpp::command_option nation_invite(dpp::co_sub_command, "invite", "Invite a player into your nation.");
                nation_command.add_option(nation_invite);

                // Subcommand /nation join.
                dpp::command_option nation_join(dpp::co_sub_command, "join", "Join a nation.");
                nation_command.add_option(nation_join);

                // Subcommand /nation kick.
                dpp::command_option nation_kick(dpp::co_sub_command, "kick", "Kick a member from your nation.");
                nation_command.add_option(nation_kick);

                // Subcommand /nation list.
                dpp::command_option nation_list(dpp::co_sub_command, "list", "Display a list with all nations of the server.");
                nation_command.add_option(nation_list);

                // Subcommand /nation rank.
                dpp::command_option nation_rank(dpp::co_sub_command, "rank", "Change the rank of a player within your nation.");
                nation_command.add_option(nation_rank);

            slash_commands.push_back(nation_command);

            // Command /kick.
            dpp::slashcommand kick_command("kick", "Deport a member from the Discord server.", bot.me.id);
            kick_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to deport.", true));
            kick_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of the deportation.", true));
            slash_commands.push_back(kick_command);

            // Command /mute.
            dpp::slashcommand mute_command("mute", "Enforce censorship on a member.", bot.me.id);
            mute_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to censor.", true));
            mute_command.add_option(dpp::command_option(dpp::co_number, "time", "Enforcement duration.", true));
            mute_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of enforcement.", true));
            slash_commands.push_back(mute_command);

            // Command /ping.
            slash_commands.push_back(dpp::slashcommand("ping", "Get the latency of the bot.", bot.me.id));

            // Command /play.
            slash_commands.push_back(dpp::slashcommand("play", "How to play to MineWorld.", bot.me.id));

            // Command /resolution.
            slash_commands.push_back(dpp::slashcommand("resolution", "Create a new United Nations resolution.", bot.me.id));

            // Command /rules.
            slash_commands.push_back(dpp::slashcommand("rules", "Display Discord and Minecraft server rules.", bot.me.id));

            // Command /unban.
            dpp::slashcommand unban_command("unban", "Revoke a permanent deportation order.", bot.me.id);
            unban_command.add_option(dpp::command_option(dpp::co_string, "user_id", "ID of the deported member.", true));
            slash_commands.push_back(unban_command);

            // Command /unmute.
            dpp::slashcommand unmute_command("unmute", "Cancel censorship sanctions of a member.", bot.me.id);
            unmute_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to free.", true));
            slash_commands.push_back(unmute_command);

            bot.global_bulk_command_create(slash_commands);

            // Bot status handler.
            const std::vector<std::string> status =
            {
                "Beta 2.0 is live, run /play!",
                "Spying on you.",
                "Obey and you will not get deported.",
                "Monitoring global economy.",
                "Reporting bad behavior.",
                "Enforcing order worldwide.",
                "Reviewing classified documents.",
                "Closely surveilling AwesomePhoenix.",
                "Eliminating opposition.",
                "Deporting cheaters/troublemakers.",
                "Providing intel to Mossad.",
                "Censoring West propaganda.",
                "Enforcing sanctions.",
                "Censoring non State-approved stuff."
            };

            int index = 0;

            bot.start_timer([&bot, status, &index](const dpp::timer &timer)
            {
                index += 1;

                if (index >= status.size())
                    index = 0;

                bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, status[index]));
            }, 4);
	    }
    });

    Utils::Logs::log("ФСБ successfully booted up!");
}
