#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <map>
#include <mysql/mysql.h>
#include <string>
#include <variant>
#include <vector>

namespace Database
{
    using VariantType = std::variant<std::nullptr_t, bool, int64_t, double, std::string>;
    using QueryData = std::vector<std::map<std::string, VariantType>>;

    ///////////////////////////////////////////////////
    //////////////////// Functions ////////////////////
    ///////////////////////////////////////////////////

    //////////////////////////////////
    ///// connection.handler.cpp /////
    //////////////////////////////////

    void connect_database
    (
        MYSQL*            &database_obj,
        const std::string &database_name,
        const std::string &host,
        const int         &port,
        const std::string &user,
        const std::string &password
    );

    void disconnect_database
    (
        MYSQL* &database_obj
    );

    ///////////////////////////////
    ///// queries.handler.cpp /////
    ///////////////////////////////

    QueryData db_query
    (
        MYSQL*            &database,
        const std::string &query
    );

    /////////////////////////////////////////////////
    //////////////////// Classes ////////////////////
    /////////////////////////////////////////////////

    //////////////////////////////////
    ///// connection.handler.cpp /////
    //////////////////////////////////

    class database_handler
    {
        public:
            database_handler
            (
                MYSQL*            &database_obj,
                const std::string &database_name,
                const std::string &host,
                const int         &port,
                const std::string &user,
                const std::string &password
            );

            ~database_handler();

            // Prevent data duplication.
            database_handler(const database_handler&) = delete;
            database_handler &operator = (const database_handler&) = delete;
        private:
            MYSQL* database;
    };
}

#endif
