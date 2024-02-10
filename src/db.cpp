#include "db.hpp"
#include <pqxx/pqxx>
#include <iostream>
#include "parser.hpp"
#include "chain_utils.hpp"

#ifndef DB_CPP
#define DB_CPP

void Database::connect(const std::string &dbname, const std::string &user, const std::string &password, const std::string &host, std::string port)
{
    try
    {
        const std::string connection_string =
            "dbname=" + dbname +
            " user=" + user +
            " password=" + password +
            " host=/cloudsql/" + host +
            " port=" + port;

        for (size_t i = 0; i < 10; ++i)
        {
            auto conn = std::make_unique<pqxx::connection>(connection_string);
            connectionPool.push(std::move(conn));
        }
    }

    catch (std::exception &e)
    {
        throw;
    }
}


std::optional<json> Database::fetchAllBlocks() {

    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        auto result = tx.exec("SELECT * FROM blocks");
        json retVal({});

        if (result.empty())
        {
            return retVal;
        }

        std::vector<json> jsonBlocksVec{result.size()};
        for (const pqxx::row &row : result)
        {
            jsonBlocksVec.push_back(Parser::row_to_json(row));
        }

        // Convert blocks to json array
        retVal = jsonBlocksVec;

        return retVal;
    }
    catch (std::exception &e)
    {
        throw;
    }
}

json Database::fetchAllTransactions()
{
    try

    {
        ManagedConnection conn(*this);
        json retVal{{}};
        transaction tx(*conn);
        auto result = tx.exec("SELECT * FROM transactions");
        if (result.empty())
        {
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
        throw;
    }
}

std::optional<json> Database::fetchPaginatedBlocks(int page, int limit, bool reverseOrder)
{

    std::optional<uint64_t> optionalTotalCount = this->fetchTotalBlocksCount();
    json retVal = {
        {"data", json::array()},
        {"totalCount", 0},
        {"totalPages", 0}};

    if (!optionalTotalCount.has_value() || limit <= 0)
    {
        return retVal; // Return early if no transactions or invalid limit.
    }

    uint64_t totalBlockCount = optionalTotalCount.value();
    uint64_t totalPages = (totalBlockCount + limit - 1) / limit; // Ceiling of totalTransactionsCount divided by limit.
    retVal["totalPages"] = totalPages;

    if (page > totalPages)
        page = totalPages; // Adjust page number if out of bounds.

    int offset;
    std::string query;
    if (reverseOrder)
    {
        // Offset backward order
        // The first page in reverse order should fetch the most recent records
        int offsetForReverse = std::max(0, static_cast<int>(totalBlockCount - (page * limit)));

        // Adjust the offset if it's beyond the total count, which can happen in the last page of reverse order.
        if (offsetForReverse < 0)
        {
            limit += offsetForReverse; // Reduce the limit to fetch only the remaining records.
            offsetForReverse = 0;      // Start from the first record.
        }

        // Elements in descending order
        query = "SELECT * FROM blocks ORDER BY CAST(height AS INTEGER) DESC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offsetForReverse);
    }
    else
    {
        // Offset forward order
        int offsetForForward = (page - 1) * limit;
        query = "SELECT * FROM blocks ORDER BY CAST(height AS INTEGER) ASC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offsetForForward);
    }

    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        auto result = tx.exec(query);

        if (result.empty())
        {
            return retVal;
        }

        std::vector<json> jsonBlocksVec;
        jsonBlocksVec.reserve(result.size());

        for (const pqxx::row &row : result)
        {
            jsonBlocksVec.push_back(Parser::row_to_json(row));
        }

        retVal["data"] = jsonBlocksVec;
        retVal["totalCount"] = totalBlockCount;
        retVal["totalPages"] = std::ceil(static_cast<double>(totalBlockCount) / limit);

        return retVal;
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::fetchPaginatedTransactions(int page, int limit, bool isReversed)
{
    std::optional<uint64_t> optionalTotalCount = this->fetchTotalTransactionCount();
    json retVal = {
        {"data", json::array()},
        {"totalCount", 0},
        {"totalPages", 0}};

    if (!optionalTotalCount.has_value() || limit <= 0)
    {
        return retVal; // Return early if no transactions or invalid limit.
    }

    uint64_t totalTransactionsCount = optionalTotalCount.value();
    uint64_t totalPages = (totalTransactionsCount + limit - 1) / limit; // Ceiling of totalTransactionsCount divided by limit.
    retVal["totalPages"] = totalPages;

    if (page > totalPages)
        page = totalPages; // Adjust page number if out of bounds.

    int offset;
    std::string query;
    if (isReversed)
    {
        // Correctly calculate the offset for reverse pagination.
        // The first page in reverse order should fetch the most recent records.
        int offsetForReverse = std::max(0, static_cast<int>(totalTransactionsCount - (page * limit)));

        // Adjust the offset if it's beyond the total count, which can happen in the last page of reverse order.
        if (offsetForReverse < 0)
        {
            limit += offsetForReverse; // Reduce the limit to fetch only the remaining records.
            offsetForReverse = 0;      // Start from the first record.
        }

        // Fetch records in descending order without reordering them in a subquery.
        query = "SELECT * FROM transactions ORDER BY CAST(height AS INTEGER) DESC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offsetForReverse);
    }
    else
    {
        int offsetForForward = (page - 1) * limit;
        query = "SELECT * FROM transactions ORDER BY CAST(height AS INTEGER) ASC LIMIT " +
                std::to_string(limit) + " OFFSET " + std::to_string(offsetForForward);
    }


    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        auto result = tx.exec(query);

        std::vector<json> jsonTxVec;
        jsonTxVec.reserve(result.size());

        for (const pqxx::row &row : result)
        {
            jsonTxVec.push_back(Parser::row_to_json(row));
        }

        retVal["data"] = jsonTxVec;
        retVal["totalCount"] = totalTransactionsCount;
        retVal["totalPages"] = std::ceil(static_cast<double>(totalTransactionsCount) / limit);

        return retVal;
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::optional<uint64_t> Database::fetchTotalTransactionCount()
{
    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        auto result = tx.exec("SELECT COUNT(*) FROM transactions");

        int retVal = 0;
        if (result.empty())
        {
            return retVal;
        }

        retVal = result[0][0].as<uint64_t>();

        return retVal;
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::optional<uint64_t> Database::getMostRecentTransactionTimestamp()
{


    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        auto result = tx.exec("SELECT MAX(timestamp) FROM transactions");

        if (result.empty() || result[0][0].is_null())
        {
            return std::nullopt;
        }

        uint64_t mostRecentTimestamp = result[0][0].as<uint64_t>();

        return mostRecentTimestamp;
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::optional<uint64_t> Database::fetchTotalBlocksCount()
{
    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        auto result = tx.exec("SELECT COUNT(*) FROM blocks");

        uint64_t retVal = 0;
        if (result.empty())
        {
            return retVal;
        }

        retVal = result[0][0].as<uint64_t>();

        return retVal;
    }
    catch (std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::fetchBlockByHash(const std::string &block_hash)
{

    try
    {
        ManagedConnection conn(*this);
        transaction txn(*conn);
        std::string query = "SELECT * FROM blocks WHERE hash = " + txn.quote(block_hash);
        auto result = txn.exec(query);
        json retVal{{}};

        if (result.empty())
        {
            return retVal;
        }

        return Parser::row_to_json(result[0]).dump();
    }
    catch (const std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::fetchTransactionByHash(const std::string &transaction_hash)
{
    try
    {
        ManagedConnection conn(*this);
        transaction txn(*conn);
        std::string query = "SELECT * FROM transactions WHERE tx_id = " + txn.quote(transaction_hash);
        auto result = txn.exec(query);
        json retVal{{}};

        if (!result.empty())
        {
            retVal;
        }

        return Parser::row_to_json(result[0]).dump();
    }
    catch (const std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::fetchTransparentOutputsRelatedToTransactionId(const std::string &transaction_id)
{

    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        std::string query = "SELECT * FROM transparent_outputs WHERE tx_id = $1";
        auto result = tx.exec_params(query, transaction_id);
        json retVal{{}};

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
        throw;
    }
}

std::optional<json> Database::fetchTransparentInputsRelatedToTransactionId(const std::string &transaction_id)
{
    try
    {
        ManagedConnection conn(*this);
        transaction tx(*conn);
        std::string query = "SELECT * FROM transparent_inputs WHERE tx_id = $1";
        auto result = tx.exec_params(query, transaction_id);
        json retVal{{}};

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
        throw;
    }
}

std::optional<json> Database::fetchTransactionsDetailsFromIds(const std::vector<std::string> &transaction_ids)
{
    json retVal{{}};
    if (transaction_ids.empty())
    {
        return retVal;
    }

    try
    {
        ManagedConnection conn(*this);
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

        retVal = transaction_details;
        return retVal;
    }
    catch (const std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::fetchPeerInfo()
{
    try
    {
        ManagedConnection connection(*this);
        json retVal{{}};
        transaction tx{*connection};
        auto result = tx.exec("SELECT * FROM peerinfo");
        std::vector<json> peer_info_list{result.size()};

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
        throw;
    }
}

std::optional<json> Database::fetchBlockchainInfo()
{

    try
    {
        ManagedConnection connection(*this);
        json retVal{{}};
        transaction tx(*connection);

        auto result = tx.exec("SELECT * FROM chain_info");

        if (result.empty())
        {
            return retVal;
        }

        return Parser::row_to_json(result[0]).dump();
    }
    catch (const std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::directSearch(const std::string &pattern)
{

    if (!isValidSHA256Hash(pattern))
    {
        throw std::runtime_error("Invalid request for pattern " + pattern + ".");
    }

    const std::string preparedStmt{"direct_search_query"};

    try
    {
        ManagedConnection connection(*this);
        transaction tx{*connection};

        connection->prepare(preparedStmt, "SELECT 'transactions' AS source_table, tx_id AS identifier FROM transactions WHERE tx_id = $1 UNION ALL SELECT 'blocks', hash FROM blocks WHERE hash = $1");

        auto result = tx.exec_prepared(preparedStmt, pattern);

        if (result.empty())
        {
            return std::nullopt;
        }

        return Parser::row_to_json(result[0]).dump();
    }
    catch (const std::exception &e)
    {
        throw;
    }
}

std::optional<json> Database::fetchTransactionInPeriod(uint64_t startTimestamp, uint64_t a)
{
    try
    {
        ManagedConnection connection(*this);
        // Calculate end timestamp
        uint64_t endTimestamp = startTimestamp + (14 * 24 * 60 * 60); // 14 days in seconds

        // Execute the SQL query to get transactions within the specified period
        transaction tx(*connection);
        auto result = tx.exec("SELECT * FROM transactions WHERE timestamp >= " +
                              std::to_string(startTimestamp) + " AND timestamp <= " +
                              std::to_string(endTimestamp));

        if (result.empty())
        {
            return json({}); // return an empty json object
        }

        // Process the result
        std::map<std::string, int> dailyTransactionCounts;
        for (const auto &row : result)
        {
            uint64_t timestamp = row["timestamp"].as<uint64_t>();
            std::string dateKey = unixTimestampToDateString(timestamp);
            dailyTransactionCounts[dateKey]++;
        }

        // Convert the accumulated data to the required format
        json formattedData;
        formattedData["data"] = std::vector<json>{};
        formattedData["startTimestamp"] = startTimestamp;
        formattedData["endTimestamp"] = endTimestamp;

        for (const auto &countEntry : dailyTransactionCounts)
        {
            formattedData["data"].push_back({{"timestamp", countEntry.first}, {"transaction_count", countEntry.second}});
        }

        return formattedData;
    }
    catch (const std::exception &e)
    {
        throw;
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
    try
    {
        std::lock_guard<std::mutex> lock(cs_pool_mutex);

        // TODO: Refactor to wait until the connection pool has > 1 pqxx::connection
        if (connectionPool.empty())
        {
            return nullptr;
        }

        auto conn = std::move(connectionPool.front());
        connectionPool.pop();

        return conn;
    }
    catch (const std::exception &e)
    {
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