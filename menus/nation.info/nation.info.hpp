#ifndef MENUS_NATIONINFO_HPP
#define MENUS_NATIONINFO_HPP

#include <dpp/dpp.h>
#include <mysql/mysql.h>

namespace NationInfo
{
    /////////////////////
    ///// media.cpp /////
    /////////////////////

    void media
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::select_click_t &event
    );

    ////////////////////////
    ///// overview.cpp /////
    ////////////////////////

    void overview
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::select_click_t &event
    );

    ////////////////////////
    ///// politics.cpp /////
    ////////////////////////

    void politics
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::select_click_t &event
    );

    //////////////////////////
    ///// population.cpp /////
    //////////////////////////

    void population
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::select_click_t &event
    );

    //////////////////
    ///// un.cpp /////
    //////////////////

    void united_nations
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::select_click_t &event
    );

    ///////////////////////
    ///// warfare.cpp /////
    ///////////////////////

    void warfare
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::select_click_t &event
    );
}

#endif
