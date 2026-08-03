#include "utils.database.hpp"

#include <mysql/mysql.h>
#include <string>

/*
    Sanitize an input to prevent SQL injection.

    Tasks:
        1)

    Parameters:
        - database / MYSQL* / FSB + MineWorld database.
        - input    / string / Data to sanitize.

    Returns:
        A string containing the sanitized input.
*/
std::string Database::sanitize_input
(
    MYSQL*            &database,
    const std::string &input
)
{
    std::string sanitized;
    sanitized.resize(input.length() * 2 + 1);

    const unsigned long length = mysql_real_escape_string(database, sanitized.data(), input.c_str(), static_cast<unsigned long>(input.length()));
    sanitized.resize(length);

    return sanitized;
}
