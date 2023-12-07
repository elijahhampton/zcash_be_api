#include "db.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include "parser.hpp"

#ifndef DB_CPP
#define DB_CPP

// DB should only return std::nullopt if something seriously goes wrong during the query.
// Otherwise please simply return a default value wrapped in json and throw 404 if data is not found.

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

std::cout << "Connect()" << std::endl;
        for (size_t i = 0; i < 10; ++i)
        {
            auto conn = std::make_unique<pqxx::connection>(connection_string);
            connectionPool.push(std::move(conn));
        }
        std::cout << "Database connected" << std::endl;
        
    }
    catch (std::exception &e)
    {
                std::cout << e.what() << std::endl;
        throw e;
    }
}

/**
 * Fetches all blocks
 */
std::optional<json> Database::fetchAllBlocks()
{
    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn.get());
        auto result = tx.exec("SELECT * FROM blocks");
        json retVal({});

        if (result.empty()) {
            return retVal;
        }

        std::vector<json> jsonBlocksVec{result.size()};
        for (const pqxx::row &row : result)
        {
            jsonBlocksVec.push_back(Parser::row_to_json(row));
        }

        // Convert blocks to json array
        retVal = jsonBlocksVec;

        this->ReleaseConnection(std::move(conn));
        return retVal;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

/**
 * Fetch all transactions
 */
json Database::fetchAllTransactions()
{
    auto conn = this->GetConnection();

    try

    {
        json retVal{{}};
        transaction tx(*conn);
        auto result = tx.exec("SELECT * FROM transactions");
        this->ReleaseConnection(std::move(conn));
        if (result.empty()) {
            return retVal;
        }

        std::vector<json> jsonTxVec;
        for (const pqxx::row &row : result)
        {
            jsonTxVec.push_back(Parser::row_to_json(row));
        }


        retVal = jsonTxVec;

        return retVal;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

// Fetch blocks with pagination
std::optional<json> Database::fetchPaginatedBlocks(int page, int limit, bool reverseOrder)
{

    std::optional<uint64_t> optionalResult = this->fetchTotalBlocksCount();
    json retVal{{}};

    if (!optionalResult.has_value())
    {
        return retVal;
    }

    uint64_t totalBlockCount = optionalResult.value();

    std::string query;
    if (reverseOrder)
    {
        // Calculate the offset from the back of the list.
        int offset = totalBlockCount - (page * limit);
        // If the offset is negative, we've gone past the beginning of the list.
        if (offset < 0)
        {
            limit += offset; // Adjust the limit to fetch the remaining records.
            offset = 0;      // Set the offset to the start of the list.
        }

        query = "SELECT * FROM (SELECT * FROM blocks ORDER BY height DESC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offset) +
                ") sub ORDER BY height ASC";
    }
    else
    {
        // Calculate the offset as normal for non-reversed order.
        int offset = (page - 1) * limit;
        query = "SELECT * FROM blocks ORDER BY height ASC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offset);
    }

    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn.get());
        auto result = tx.exec(query);

        if (result.empty()) {
            return retVal;
        }

        std::vector<json> jsonBlocksVec;
        for (const pqxx::row &row : result)
        {
            jsonBlocksVec.push_back(Parser::row_to_json(row));
        }

        retVal = jsonBlocksVec;

        this->ReleaseConnection(std::move(conn));

        return retVal;
    }
    catch (std::exception &e)
    {

        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchPaginatedTransactions(int page, int limit, bool isReversed)
{
    std::optional<uint64_t> optionalResult = this->fetchTotalTransactionCount();
    json retVal{{}};

    if (!optionalResult.has_value())
    {
        return retVal;
    }

    uint64_t totalTransactionsCount = optionalResult.value();

    std::string query;
    if (isReversed)
    {
        // Calculate the offset from the back of the list.
        int offset = totalTransactionsCount - (page * limit);
        // If the offset is negative, we've gone past the beginning of the list.
        if (offset < 0)
        {
            limit += offset; // Adjust the limit to fetch the remaining records.
            offset = 0;      // Set the offset to the start of the list.
        }

        query = "SELECT * FROM (SELECT * FROM transactions ORDER BY height DESC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offset) +
                ") sub ORDER BY height ASC";
    }
    else
    {
        // Calculate the offset as normal for non-reversed order.
        int offset = (page - 1) * limit;
        query = "SELECT * FROM transactions ORDER BY height ASC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offset);
    }

    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn);
        auto result = tx.exec(query);

        std::vector<json> jsonTxVec;
        for (const pqxx::row &row : result)
        {
            jsonTxVec.push_back(Parser::row_to_json(row));
        }

        retVal = jsonTxVec;

        this->ReleaseConnection(std::move(conn));

        return retVal;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

// Function to fetch the total count of records from the Transactions table.
std::optional<uint64_t> Database::fetchTotalTransactionCount()
{
    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn);
        auto result = tx.exec("SELECT COUNT(*) FROM transactions");
        
        int retVal = 0;
        if (result.empty())
        {
            return retVal;
        }

        retVal = result[0][0].as<uint64_t>();

        this->ReleaseConnection(std::move(conn));
        return retVal;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

// Function to fetch the total count of records from the Blocks table.
std::optional<uint64_t> Database::fetchTotalBlocksCount()
{
    auto conn = this->GetConnection();
    try
    {
        transaction tx(*conn);
        auto result = tx.exec("SELECT COUNT(*) FROM blocks");

        uint64_t retVal = 0;
        if (result.empty())
        {
            return retVal;
        }

        retVal = result[0][0].as<uint64_t>();

        this->ReleaseConnection(std::move(conn));

        return retVal;
    }
    catch (std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchBlockByHash(const std::string &block_hash)
{
    auto conn = this->GetConnection();

    try
    {
        transaction txn(*conn);
        std::string query = "SELECT * FROM blocks WHERE hash = " + txn.quote(block_hash);
        auto result = txn.exec(query);
        json retVal{{}};
        this->ReleaseConnection(std::move(conn));

        if (result.empty())
        {
            return retVal;
        }

        return Parser::row_to_json(result[0]).dump();
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchTransactionByHash(const std::string &transaction_hash)
{
    auto conn = this->GetConnection();

    try
    {
        transaction txn(*conn);
        std::string query = "SELECT * FROM transactions WHERE tx_id = " + txn.quote(transaction_hash);
        auto result = txn.exec(query);
        json retVal{{}};
        this->ReleaseConnection(std::move(conn));

        if (!result.empty())
        {
           retVal;
        }

        return Parser::row_to_json(result[0]).dump(); 
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchTransparentOutputsRelatedToTransactionId(const std::string &transaction_id)
{
    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn);
        std::string query = "SELECT * FROM transparent_outputs WHERE tx_id = $1";
        auto result = tx.exec_params(query, transaction_id);
        json retVal{{}};
        this->ReleaseConnection(std::move(conn));

        if (result.empty())
        {
            return retVal;
        }

        std::vector<json> transaction_outputs;
        for (const auto &row : result)
        {
                transaction_outputs.push_back(Parser::row_to_json(row));
        }

        retVal = transaction_outputs;
        return retVal;
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchTransparentInputsRelatedToTransactionId(const std::string &transaction_id)
{
    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn);
        std::string query = "SELECT * FROM transparent_inputs WHERE tx_id = $1";
        auto result = tx.exec_params(query, transaction_id);
        json retVal{{}};
        this->ReleaseConnection(std::move(conn));

        if (result.empty())
        {
            return retVal;
        }

        std::vector<json> transaction_inputs;
        for (const auto &row : result)
            {
                transaction_inputs.push_back(Parser::row_to_json(row));
            }

        retVal = transaction_inputs;
        return retVal;
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchTransactionsDetailsFromIds(const std::vector<std::string> &transaction_ids)
{
    json retVal{{}};
    if (transaction_ids.empty())
    {
        return retVal;
    }

    auto conn = this->GetConnection();

    try
    {
        transaction tx(*conn);
        std::string query{""};
        pqxx::result res;
        std::vector<json> transaction_details;
        json retVal{{}};
        for (const std::string &id : transaction_ids)
        {
            query = "SELECT * FROM transactions WHERE tx_id = $1";
            res = tx.exec_params(query, id);

            if (!res.empty())
            {
                for (const auto &row : res)
                {
                    transaction_details.push_back(Parser::row_to_json(row));
                }
            }
        }
        this->ReleaseConnection(std::move(conn));

        retVal = transaction_details;
        return retVal;
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(conn));
        throw e;
    }
}

std::optional<json> Database::fetchPeerInfo()
{
    auto connection = this->GetConnection();

    try
    {
        json retVal{{}};
        transaction tx{*connection.get()};
        auto result = tx.exec("SELECT * FROM peerinfo");
        std::vector<json> peer_info_list{result.size()};
        this->ReleaseConnection(std::move(connection));

        if (result.empty())
        {
            return retVal;
        }

        for (const auto &row : result)
        {
            peer_info_list.push_back(Parser::row_to_json(row));
        }

        retVal = peer_info_list;

        return retVal;
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(connection));
        throw e;
    }
}

std::optional<json> Database::fetchBlockchainInfo()
{
    std::unique_ptr<pqxx::connection> connection = this->GetConnection();

    try
    {
        json retVal{{}};
        transaction tx(*connection.get());

        auto result = tx.exec("SELECT * FROM chain_info");
        this->ReleaseConnection(std::move(connection));

        if (result.empty())
        {
            return retVal;
        }

        return Parser::row_to_json(result[0]).dump();
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(connection));
        throw e;
    }
}

std::optional<json> Database::fetchTransactionsForLast14Days(uint64_t startTimestamp, uint64_t a)
{
     std::unique_ptr<pqxx::connection> connection = this->GetConnection();

    try
    {
        json retVal{{}};
        transaction tx(*connection.get());
        auto result = tx.exec("SELECT * FROM transactions");
        this->ReleaseConnection(std::move(connection));

        if (result.empty())
        {
            return retVal;
        }

        // Convert the result to a vector of json objects
        std::vector<json> transactionData{result.size()};
        json entry;
        for (const auto &row : result)
        {
            entry["timestamp"] = row["timestamp"].as<std::string>();
            transactionData.push_back(entry);
            entry.clear();
        }

        // Filter transactions based on the 14-day period from the start timestamp
        std::vector<json> filteredData;
        uint64_t endTimestamp = startTimestamp + (14 * 24 * 60 * 60); // 14 days in seconds
        
        uint64_t entryTimestamp{0};
        for (const auto &entry : transactionData)
        {
            entryTimestamp = std::stoull(entry["timestamp"].get<std::string>());
            if (entryTimestamp >= startTimestamp && entryTimestamp <= endTimestamp)
            {
                filteredData.push_back(entry);
            }
        }

        std::map<std::string, int> dailyTransactionCounts;
        std::string dateKey{""};
        for (const auto &entry : filteredData)
        {
            entryTimestamp = std::stoull(entry["timestamp"].get<std::string>());
            
            // Convert timestamp to "YYYY-MM-DD" format
            dateKey = unixTimestampToDateString(entryTimestamp);

            dailyTransactionCounts[dateKey] += 1; 
        }

        // Convert the accumulated data to the required format
        std::vector<json> formattedData;
        json dataEntry;
        for (const auto &countEntry : dailyTransactionCounts)
        {
            dataEntry["timestamp"] = countEntry.first;
            dataEntry["transaction_count"] = countEntry.second;
            formattedData.push_back(dataEntry);
            dataEntry.clear();
        }

        return formattedData;
    }
    catch (const std::exception &e)
    {
        this->ReleaseConnection(std::move(connection));
        throw e;
    }
}

std::string Database::unixTimestampToDateString(uint64_t timestamp)
{
    std::time_t time = static_cast<std::time_t>(timestamp);
    std::tm *tmStruct = std::localtime(&time);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tmStruct);
    return std::string(buffer);
}

std::unique_ptr<pqxx::connection> Database::GetConnection()
{
    try {
        std::lock_guard<std::mutex> lock(cs_pool_mutex);

        // TODO: Refactor to wait until the connection pool has > 1 pqxx::connection
        if (connectionPool.empty())
        {
            return nullptr;
        }

        auto conn = std::move(connectionPool.front());
        connectionPool.pop();

        return conn;
    } catch(const std::exception& e) {
        throw std::runtime_error(e.what());
    }
}

void Database::createJsonErrorResponse(json &errorResponse, const std::exception &e)
{
    errorResponse["error"] = e.what();
}

bool Database::ReleaseConnection(std::unique_ptr<pqxx::connection> conn)
{
    std::lock_guard<std::mutex> lock(cs_pool_mutex);
    connectionPool.push(std::move(conn));
    return true;
}

void Database::ShutdownConnections()
{
    // TODO
}

#endif // DB_CPP