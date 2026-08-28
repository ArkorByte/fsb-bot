#include "miscellaneous.hpp"

#include <string>
#include <time.h>
#include <vector>

/*
    Get the timestamp of the actual time.

    Tasks:
        1) Get the current timestamp.

    Parameters (variable_name / type / description):
        No parameters.

    Returns (type + description):
        A time_t containg the current timestamp.
*/
time_t Miscellaneous::get_current_timestamp()
{
    const time_t timestamp = time(nullptr);
    return timestamp;
}



/*
    Get the date of a timestamp.

    Tasks:
        1) Get the time.
            a. From the timestamp, get time data for host timezone.
            b. Get time elements and fix year that counts from year 1900 and months as the range is (0 - 11).
        2) Format some time elements and prepare the output.

    Parameters:
        - timestamp / time_t / Timestamp used to get the date from.

    Returns:
        A vector list containing the date.
        Output format: year (YYYY), month (MM), day (DD), hour (HH), minutes (MM), seconds (SS).
*/
std::vector<std::string> Miscellaneous::get_date_from_timestamp
(
    const time_t &timestamp
)
{
    ////////////////// 1) //////////////////
    ///////// a. /////////
    const struct tm date = *localtime(&timestamp);

    ///////// b. /////////
    const std::string year = std::to_string(date.tm_year + 1900); // date.tm_year returns the amount of years that passed since 1900.
    const std::string month = std::to_string(date.tm_mon + 1);    // date.tm_mon returns the month number from 0 to 11.
    const std::string day = std::to_string(date.tm_mday);
    const std::string hour = std::to_string(date.tm_hour);
    const std::string minutes = std::to_string(date.tm_min);
    const std::string seconds = std::to_string(date.tm_sec);

    ////////////////// 2) //////////////////
    const std::vector<std::string> output =
    {
        year,
        month.size() < 2 ? "0" + month : month,
        day.size() < 2 ? "0" + day : day,
        hour.size() < 2 ? "0" + hour : hour,
        minutes.size() < 2 ? "0" + minutes : minutes,
        seconds.size() < 2 ? "0" + seconds : seconds
    };

    return output;
}
