#ifndef UTILS_MISCELLANEOUS_HPP
#define UTILS_MISCELLANEOUS_HPP

#include <dpp/dpp.h>
#include <string>
#include <time.h>
#include <vector>

namespace Miscellaneous
{
    ////////////////////////
    ///// integers.cpp /////
    ////////////////////////

    bool is_an_integer
    (
        const std::string &input
    );

    /////////////////////////////
    ///// role.position.cpp /////
    /////////////////////////////

    uint8_t highest_role_position
    (
        const dpp::guild_member &member
    );

    ////////////////////
    ///// time.cpp /////
    ////////////////////

    time_t get_current_timestamp();

    std::vector<std::string> get_date_from_timestamp
    (
        const time_t &timestamp
    );
}

#endif
