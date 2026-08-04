#ifndef AUTOCOMPLETE_HPP
#define AUTOCOMPLETE_HPP

#include <dpp/dpp.h>
#include <mysql/mysql.h>

namespace Autocomplete
{
    /////////////////////////////
    ///// empty.nations.cpp /////
    /////////////////////////////

    void empty_nations
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::autocomplete_t &event
    );

    ///////////////////////
    ///// nations.cpp /////
    ///////////////////////

    void nations
    (
        dpp::cluster              &bot,
        MYSQL*                    &database,
        const dpp::autocomplete_t &event
    );

    /////////////////////
    ///// ranks.cpp /////
    /////////////////////

    void ranks
    (
        dpp::cluster              &bot,
        const dpp::autocomplete_t &event
    );
}

#endif
