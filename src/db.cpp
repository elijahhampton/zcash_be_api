#include "db.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include "parser.hpp"

#ifndef DB_CPP
#define DB_CPP

Database::Database()
{
}

Database::~Database() {}

/**
 * Connects to a postgres database instance
 */
void Database::connect(const std::string &dbname, const std::string &user, const std::string &password, const std::string &host, std::string port)
{
    try
    {
        const std::string connection_string =
            "dbname=" + dbname +
            " user=" + user +
            " password=" + password +
            " host=" + host +
            " port=" + port;

        for (size_t i = 0; i < 10; ++i)
        {
            auto conn = std::make_unique<pqxx::connection>(connection_string);
            connectionPool.push(std::move(conn));
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}


/**
 * Fetches all blocks
 */
std::optional<json> Database::fetchAllBlocks()
{
    std::unique_ptr<pqxx::connection> conn = this->GetConnection();
    std::vector<json> jsonBlocksVec;
    try

    {
        pqxx::work tx(*conn.get());
        pqxx::result result = tx.exec("SELECT * FROM blocks");

        for (const pqxx::row &row : result)
        {
            jsonBlocksVec.push_back(Parser::row_to_json(row));
        }

        // Convert blocks to json array
        json vecAsJson = jsonBlocksVec;
        tx.commit();
        this->ReleaseConnection(std::move(conn));
        return vecAsJson;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        std::cout << e.what() << std::endl;
        return json(nullptr);
    }
}

/**
 * Fetch all transactions
 */
json Database::fetchAllTransactions()
{
    std::unique_ptr<pqxx::connection> conn = this->GetConnection();
    std::vector<json> jsonTxVec;
    try

    {
        pqxx::work tx(*conn);
        pqxx::result result = tx.exec("SELECT * FROM transactions");

        for (const pqxx::row &row : result)
        {
            jsonTxVec.push_back(Parser::row_to_json(row));
        }

        // Convert transactions to json array
        json vecAsJson = jsonTxVec;
        tx.commit();

        this->ReleaseConnection(std::move(conn));
        return vecAsJson;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        std::cout << e.what() << std::endl;
        return json(nullptr);
    }
}

std::unique_ptr<pqxx::connection> Database::GetConnection()
{
    std::lock_guard<std::mutex> lock(poolMutex);
    if (connectionPool.empty())
    {
        return nullptr;
    }
    auto conn = std::move(connectionPool.front());
    connectionPool.pop();
    return conn;
}

bool Database::ReleaseConnection(std::unique_ptr<pqxx::connection> conn)
{
    std::lock_guard<std::mutex> lock(poolMutex);
    connectionPool.push(std::move(conn));
}

void Database::ShutdownConnections()
{
    // TODO
}

json Database::fetchTransactionsSinceHeight(uint32_t lastBlockHeight) {}

json Database::fetchBlocksSinceHeight(uint32_t lastBlockHeight) {}

#endif // DB_CPP