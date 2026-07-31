#ifndef MODALS_HPP
#define MODALS_HPP

#include <dpp/dpp.h>
#include <mysql/mysql.h>

namespace Modals
{
    //////////////////////////
    ///// journalism.cpp /////
    //////////////////////////

    void journalism
    (
        dpp::cluster             &bot,
        MYSQL*                   &database,
        const dpp::form_submit_t &event
    );
}

#endif
