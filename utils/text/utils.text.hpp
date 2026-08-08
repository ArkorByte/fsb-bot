#ifndef UTILS_TEXT_HPP
#define UTILS_TEXT_HPP

#include <string>

namespace Text
{
    /////////////////////
    ///// emoji.cpp /////
    /////////////////////

    std::string get_nation_flag
    (
        const std::string &nation_id
    );

    //////////////////////////
    ///// nations.db.cpp /////
    //////////////////////////

    std::string get_government_type
    (
        const int &index
    );

    std::string get_ideology
    (
        const int &index
    );

    std::string get_join_confition
    (
        const int &index
    );

    std::string get_rank
    (
        const int &score
    );

    std::string get_media_freedom_rating
    (
        const int &score
    );

    ////////////////////
    ///// trim.cpp /////
    ////////////////////

    std::string trim
    (
        const std::string &input
    );
}

#endif
