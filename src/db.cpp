#include "db.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include "parser.hpp"

#ifndef DB_CPP
#define DB_CPP

Database::Database() {

}

Database::~Database() {}

/**
 * Connects to a postgres database instance
*/
void Database::connect()
{
    try
    {
        std::string connection_string = "dbname=postgres user=postgres password=mysecretpassword host=127.0.0.1 port=5432"; //this->loadConfig("../config/db_config.cfg")
        conn = std::make_unique<pqxx::connection>(connection_string);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

/**
 * Disconnects a postgres database instance
*/
void Database::disconnect()
{
    if (conn->is_open())
    {
        conn->close();
    }
}

/**
 * Load the configuration file
*/
std::string Database::loadConfig(const std::string &path)
{
    std::ifstream cfg_file(path);
    std::string conn_str;
    std::getline(cfg_file, conn_str);
    return conn_str;
}

/**
 * Fetches all blocks
*/
std::optional<json> Database::fetchAllBlocks()
{
    std::vector<json> jsonBlocksVec;
    try

    {
        pqxx::work tx(*conn);
        pqxx::result result = tx.exec("SELECT * FROM blocks");

        for (const pqxx::row& row : result)
        {
            jsonBlocksVec.push_back(Parser::row_to_json(row));
        }

        // Convert blocks to json array
        json vecAsJson = jsonBlocksVec;

        return vecAsJson;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return json(nullptr);
    }
}

/**
 * Fetch all transactions
*/
json Database::fetchAllTransactions() {
    std::vector<json> jsonTxVec;
    try

    {
        pqxx::work tx(*conn);
        pqxx::result result = tx.exec("SELECT * FROM transactions");

        for (const pqxx::row& row : result)
        {
            jsonTxVec.push_back(Parser::row_to_json(row));
        }

        // Convert transactions to json array
        json vecAsJson = jsonTxVec;

        return vecAsJson;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return json(nullptr);
    }
}

json Database::fetchTransactionsSinceHeight(uint32_t lastBlockHeight) {}

json Database::fetchBlocksSinceHeight(uint32_t lastBlockHeight) {}

#endif // DB_CPP