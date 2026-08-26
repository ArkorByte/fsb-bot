#ifndef BUTTONS_HPP
#define BUTTONS_HPP

#include <dpp/dpp.h>
#include <mysql/mysql.h>
#include <string>

namespace Buttons
{
    //////////////////////////
    ///// journalism.cpp /////
    //////////////////////////

    void journalism_censor
    (
        dpp::cluster        &bot,
        MYSQL*              &database,
        dpp::button_click_t &event,
        const std::string   &ID
    );

    void journalism_blacklist
    (
        dpp::cluster        &bot,
        MYSQL*              &database,
        dpp::button_click_t &event
    );
}

#endif
