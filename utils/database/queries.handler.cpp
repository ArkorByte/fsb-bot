#include "utils.database.hpp"

#include "../utils.hpp"

#include <mysql/mysql.h>
#include <string>

/*
    Make a query to the database.

    Tasks:
        1) Verify that database is valid.
        2) Try to make a query to the provided database.
        3) Retrieve the result of our query that can be data from a table.
        4) If we have no results:
            - Either nothing went wrong and no data was expected.
            - Data was expected but nothing was retrieved.
        5) Register the names of the columns for the map keys.
        6) We register the data of the rows and associate it to the previously registered column names.
        7) Free allocated memory.

    Parameters:
        - database / MYSQL* / Database to query.
        - query    / string / Query to send to the database.

    Returns:
        A vector list containing maps of strings that represents the output data from the query.
*/
Database::QueryData Database::db_query
(
    MYSQL*            &database,
    const std::string &query
)
{
    Database::QueryData output;

    if (database == nullptr)
    {
        Logs::log("Warning: Query \"" + query + "\" failed! Invalid database provided.");
        return output;
    }

    const int query_result = mysql_query(database, query.c_str());

    if (query_result != 0)
    {
        Logs::log("Warning: Query \"" + query + "\" failed with error code -> " + std::string(mysql_error(database)) + ".");
        return output;
    }

    MYSQL_RES* result = mysql_store_result(database);

    if (result == nullptr)
    {
        if (mysql_field_count(database) > 0)
            Logs::log("Warning: Failed to get query result with error code -> " + std::string(mysql_error(database)) + ".");

        return output;
    }

    int fields_count = mysql_num_fields(result);
    const MYSQL_FIELD* fields = mysql_fetch_fields(result);

    std::vector<std::string> column_names;

    for (int i = 0; i < fields_count; i++)
        column_names.push_back(fields[i].name);

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)) != nullptr)
    {
        std::map<std::string, std::string> row_data;

        for (int i = 0; i < fields_count; i++)
            row_data[column_names[i]] = row[i];

        output.push_back(row_data);
    }

    mysql_free_result(result);
    return output;
}
