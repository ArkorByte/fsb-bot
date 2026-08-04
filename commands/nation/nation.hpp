#ifndef COMMANDS_NATION_HPP
#define COMMANDS_NATION_HPP

#include <dpp/dpp.h>
#include <mysql/mysql.h>

namespace Nation
{
    ////////////////////////////
    ///// nation.claim.cpp /////
    ////////////////////////////

    void claim_nation
    (
        dpp::cluster                    &bot,
        MYSQL*                          &database,
        const dpp::interaction_create_t &event
    );

    ///////////////////////////
    ///// nation.info.cpp /////
    ///////////////////////////

    void nation_info
    (
        dpp::cluster                    &bot,
        MYSQL*                          &database,
        const dpp::interaction_create_t &event
    );
}

#endif
