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
                alliance_config.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_config);

                // Subcommand /alliance create.
                dpp::command_option alliance_create(dpp::co_sub_command, "create", "Create a new alliance.");
                alliance_create.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_create);

                // Subcommand /alliance info.
                dpp::command_option alliance_info(dpp::co_sub_command, "info", "Get information about an alliance.");
                alliance_info.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_info);

                // Subcommand /alliance invite.
                dpp::command_option alliance_invite(dpp::co_sub_command, "invite", "Invite a nation into an alliance.");
                alliance_invite.add_option(dpp::command_option(dpp::co_string, "nation_id", "Nation to invite.", true));
                alliance_invite.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_invite);

                // Subcommand /alliance join.
                dpp::command_option alliance_join(dpp::co_sub_command, "join", "Join an alliance.");
                alliance_join.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_join);

                // Subcommand /alliance kick.
                dpp::command_option alliance_kick(dpp::co_sub_command, "kick", "Kick a nation from an alliance.");
                alliance_kick.add_option(dpp::command_option(dpp::co_string, "nation_id", "Nation to kick.", true));
                alliance_kick.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_kick);

                // Subcommand /alliance leave.
                dpp::command_option alliance_leave(dpp::co_sub_command, "leave", "Leave an alliance.");
                alliance_leave.add_option(dpp::command_option(dpp::co_string, "alliance_id", "ID of the alliance.", true).set_auto_complete(true));
                alliance_command.add_option(alliance_leave);

                // Subcommand /alliance list.
                dpp::command_option alliance_list(dpp::co_sub_command, "list", "Display all alliances of the server.");
                alliance_command.add_option(alliance_list);

            slash_commands.push_back(alliance_command);

            // Command /ban.
            dpp::slashcommand ban_command("ban", "Permanently deport a member from the Discord server.", bot.me.id);
	        ban_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to deport.", true));
            ban_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of the deportation.", true));
            ban_command.add_option(dpp::command_option(dpp::co_boolean, "silent", "Display a message into the channel or not.", true));
            slash_commands.push_back(ban_command);

            // Command /config.
            slash_commands.push_back(dpp::slashcommand("config", "Edit bot configuration.", bot.me.id));

            // Command /journalism.
            slash_commands.push_back(dpp::slashcommand("journalism", "Make a post in the journalism channel.", bot.me.id));

            // Command /kick.
            dpp::slashcommand kick_command("kick", "Deport a member from the Discord server.", bot.me.id);
            kick_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to deport.", true));
            kick_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of the deportation.", true));
            kick_command.add_option(dpp::command_option(dpp::co_boolean, "silent", "Display a message into the channel or not.", true));
            slash_commands.push_back(kick_command);

            // Command /laws.
            dpp::slashcommand laws_command("laws", "Commands related to international laws.", bot.me.id);

                // Subcommand /laws details.
                dpp::command_option laws_details(dpp::command_option(dpp::co_sub_command, "details", "Show all details about an international law."));
                laws_details.add_option(dpp::command_option(dpp::co_string, "resolution_id", "ID of the resolution.", true).set_auto_complete(true));
                laws_command.add_option(laws_details);

                // Subcommand /laws list.
                dpp::command_option laws_list(dpp::command_option(dpp::co_sub_command, "list", "List all international laws."));
                laws_command.add_option(laws_list);

            slash_commands.push_back(laws_command);

            // Command /mute.
            dpp::slashcommand mute_command("mute", "Enforce full censorship on a member.", bot.me.id);
            mute_command.add_option(dpp::command_option(dpp::co_user, "member", "Member to censor.", true));
            mute_command.add_option(dpp::command_option(dpp::co_number, "time", "Enforcement duration.", true));
            mute_command.add_option(dpp::command_option(dpp::co_string, "reason", "Reason of enforcement.", true));
            slash_commands.push_back(mute_command);

            // Command /nation.
            dpp::slashcommand nation_command("nation", "Commands related to factions.", bot.me.id);

                // Subcommand /nation censorship.
                dpp::command_option nation_censorship(dpp::co_sub_command, "censorship", "Edit automatic media censorship settings.");
                nation_censorship.add_option(nation_censorship);

                // Subcommand /nation config.
                dpp::command_option nation_config(dpp::co_sub_command, "config", "Edit the configuration of your nation.");
                nation_command.add_option(nation_config);

                // Subcommand /nation claim.
                dpp::command_option nation_claim(dpp::co_sub_command, "claim", "Claim an empty nation.");
                nation_claim.add_option(dpp::command_option(dpp::co_string, "nation_id", "ID of the nation.", true).set_auto_complete(true));
                nation_command.add_option(nation_claim);

                // Subcommand /nation info.
                dpp::command_option nation_info(dpp::co_sub_command, "info", "Get information about a nation.");
                nation_info.add_option(dpp::command_option(dpp::co_string, "nation_id", "ID of the nation.", true).set_auto_complete(true));
                nation_command.add_option(nation_info);

                // Subcommand /nation invite.
                dpp::command_option nation_invite(dpp::co_sub_command, "invite", "Invite a player into your nation.");
                nation_invite.add_option(dpp::command_option(dpp::co_user, "player", "Player to invite.", true));
                nation_command.add_option(nation_invite);

                // Subcommand /nation join.
                dpp::command_option nation_join(dpp::co_sub_command, "join", "Join a nation.");
                nation_join.add_option(dpp::command_option(dpp::co_string, "nation_id", "Nation to join.", true).set_auto_complete(true));
                nation_command.add_option(nation_join);

                // Subcommand /nation kick.
                dpp::command_option nation_kick(dpp::co_sub_command, "kick", "Kick a member from your nation.");
                nation_kick.add_option(dpp::command_option(dpp::co_user, "member", "Member to kick.", true));
                nation_command.add_option(nation_kick);

                // Subcommand /nation leave.
                dpp::command_option nation_leave(dpp::co_sub_command, "leave", "Leave your nation.");
                nation_leave.add_option(nation_leave);

                // Subcommand /nation list.
                dpp::command_option nation_list(dpp::co_sub_command, "list", "Display all nations of the server.");
                nation_command.add_option(nation_list);

                // Subcommand /nation rank.
                dpp::command_option nation_rank(dpp::co_sub_command, "rank", "Change the rank of a player within your nation.");
                nation_rank.add_option(dpp::command_option(dpp::co_user, "member", "Member to edit.", true));
                nation_rank.add_option(dpp::command_option(dpp::co_string, "new_rank", "New rank for the user.", true).set_auto_complete(true));
                nation_command.add_option(nation_rank);

                // Subcommand /nation relationship.
                dpp::command_option nation_relationship(dpp::co_sub_command, "relationship", "Change relationship percentage with another nation.");
                nation_relationship.add_option(dpp::command_option(dpp::co_string, "nation_id", "ID of the nation.", true));
                nation_relationship.add_option(dpp::command_option(dpp::co_number, "percentage", "New relationship percentage (0 ~ 100).", true));
                nation_command.add_option(nation_relationship);

            slash_commands.push_back(nation_command);

            // Command /ping.
            slash_commands.push_back(dpp::slashcommand("ping", "Get the latency of the bot.", bot.me.id));

            // Command /play.
            slash_commands.push_back(dpp::slashcommand("play", "How to play to MineWorld.", bot.me.id));

            // Command /resolution.
            dpp::slashcommand resolution_command("resolution", "Create a new United Nations resolution.", bot.me.id);

                // Subcommand /resolution membership.
                dpp::command_option resolution_membership(dpp::co_sub_command, "membership", "Propose to add a country to the United Nations.");
                resolution_membership.add_option(dpp::command_option(dpp::co_string, "nation_id", "Nation to add.", true).set_auto_complete(true));
                resolution_command.add_option(resolution_membership);

                // Subcommand /resolution law.
                dpp::command_option resolution_law(dpp::co_sub_command, "law", "Propose to edit international laws.");
                resolution_law.add_option(dpp::command_option(dpp::co_string, "action", "Action to make.", true).set_auto_complete(true));
                resolution_law.add_option(dpp::command_option(dpp::co_string, "law_id", "ID of the law or to attribute.", true).set_auto_complete(true));
                resolution_command.add_option(resolution_law);

                // Subcommand /resolution sanction.
                dpp::command_option resolution_sanction(dpp::co_sub_command, "sanction", "Propose a sanction against another nation.");
                resolution_sanction.add_option(dpp::command_option(dpp::co_string, "nation_id", "Nation to sanction.", true).set_auto_complete(true));
                resolution_sanction.add_option(dpp::command_option(dpp::co_string, "sanction_type", "Sanction to apply.", true).set_auto_complete(true));
                resolution_sanction.add_option(dpp::command_option(dpp::co_string, "duration", "Duration of sanction.", true).set_auto_complete(true));
                resolution_command.add_option(resolution_sanction);

            slash_commands.push_back(resolution_command);

            // Command /relationship.
            dpp::slashcommand relationship_command("relationship", "Display the current relationship between two nations.", bot.me.id);
            relationship_command.add_option(dpp::command_option(dpp::co_string, "first_nation_id", "Nation A.", true).set_auto_complete(true));
            relationship_command.add_option(dpp::command_option(dpp::co_string, "second_nation_id", "Nation B.", true).set_auto_complete(true));
            slash_commands.push_back(relationship_command);

            // Command /rules.
            slash_commands.push_back(dpp::slashcommand("rules", "Display Discord and Minecraft server rules.", bot.me.id));

            // Command /sanctions.
            dpp::slashcommand sanctions_command("sanctions", "Commands related to United Nations sanctions.", bot.me.id);

                // Subcommand /sanctions details.
                dpp::command_option sanctions_details(dpp::command_option(dpp::co_sub_command, "details", "Show all details about a sanction."));
                sanctions_details.add_option(dpp::command_option(dpp::co_string, "resolution_id", "ID of the resolution.", true).set_auto_complete(true));
                sanctions_command.add_option(sanctions_details);

                // Subcommand /sanctions history.
                dpp::command_option sanctions_history(dpp::command_option(dpp::co_sub_command, "history", "List all sanctions that are no longer active."));
                sanctions_command.add_option(sanctions_history);

                // Subcommand /sanctions list.
                dpp::command_option sanctions_list(dpp::command_option(dpp::co_sub_command, "list", "List all on-going sanctions."));
                sanctions_command.add_option(sanctions_list);

            slash_commands.push_back(sanctions_command);

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
