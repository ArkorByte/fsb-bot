#ifndef UTILS_LOGS_HPP
#define UTILS_LOGS_HPP

#include <string>

namespace Logs
{
    ////////////////////////////
    ///// logs.handler.cpp /////
    ////////////////////////////

    void log
    (
        const std::string &message
    );

    void crash_log
    (
        const std::string &message
    );
}

#endif
