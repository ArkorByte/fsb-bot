#include "config/tweaks.hpp"
#include "events/events.hpp"
#include "utils/utils.hpp"

#include <dpp/dpp.h>
#include <filesystem>
#include <map>
#include <mysql/mysql.h>
#include <string>

/*
    
*/
int main()
{
    if (std::filesystem::exists("fsb.logs"))
        std::filesystem::remove("fsb.logs");

    if constexpr (Tweaks::ENABLE_LOGS_FILE)
        Utils::Files::create_new_empty_file("fsb.logs");

    std::map<std::string, std::string> config = Utils::Parsers::parse_config_file("fsb.config");

    const std::string token = config["TOKEN"];
    const std::string db_name = config["DB_NAME"];
    const std::string db_host = config["DB_HOST"];
    const std::string db_port_str = config["DB_PORT"];
    const std::string db_user = config["DB_USER"];
    const std::string db_password = config["DB_PASSWORD"];

    int db_port = 0;

    if (!Utils::Miscellaneous::is_an_integer(db_port_str))
    {
        Utils::Logs::log("Warning: Invalid port provided, selected MariaDB's default port 3306.");
        db_port = 3306;
    }
    else db_port = stoi(db_port_str);

    MYSQL* database = nullptr;
    Utils::Database::database_handler db_handler(database, db_name, db_host, db_port, db_user, db_password);

    const uint32_t intents = dpp::i_all_intents;
    dpp::cluster bot(token, intents);

    Events::ready(bot);

    Events::guild_member_add(bot, database);
    Events::guild_member_remove(bot);

    Events::interaction_create(bot, database);
    Events::form_submit(bot, database);
    Events::message_create(bot);

    bot.start(dpp::st_wait);
    return 0;
}
