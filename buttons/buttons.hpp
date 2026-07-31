#ifndef BUTTONS_HPP
#define BUTTONS_HPP

#include <dpp/dispatcher.h>
#include <dpp/dpp.h>
#include <mysql/mysql.h>

namespace Buttons
{
    //////////////////////////
    ///// journalism.cpp /////
    //////////////////////////

    void journalism_censor_button
    (
        dpp::cluster        &bot,
        MYSQL*              &database,
        dpp::button_click_t &event
    );

    void journalism_blacklist_button
    (
        dpp::cluster        &bot,
        MYSQL*              &database,
        dpp::button_click_t &event
    );
}

#endif
