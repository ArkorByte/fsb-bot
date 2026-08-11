#ifndef UTILS_FILES_HPP
#define UTILS_FILES_HPP

#include <string>

namespace Files
{
    ////////////////////////////
    ///// file.manager.cpp /////
    ////////////////////////////

    bool create_new_empty_file
    (
        const std::string &file_path
    );

    bool write_file
    (
        const bool        &append,
        const std::string &data,
        const std::string &file_path
    );
}

#endif
