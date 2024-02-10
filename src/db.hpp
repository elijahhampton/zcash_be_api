

#include <string>
#include <pqxx/pqxx>
#include <mutex>
#include <queue>
#include "nlohmann/json.hpp"
#include <memory>
#include <fstream>
#include <cstdint>
#include <optional>
#include "../include/crow_all.h"

using json = nlohmann::json;
using transaction = pqxx::work;

class ManagedConnection;

/**
 * @brief Database class for handling interactions with a PostgreSQL database.
 */
class Database
{
    friend struct ManagedConnection;

public:
    /**
     * @brief Constructor for the Database class.
     */
    Database() = default;

    /**
     * @brief Destructor for the Database class.
     */
    ~Database() noexcept = default;

    /**
     * @brief Connect to the PostgreSQL database.
     * @param dbname Database name.
     * @param user User name.
     * @param password User password.
     * @param host Database host.
     * @param port Database port.
     */
    void connect(const std::string &dbname, const std::string &user, const std::string &password, const std::string &host, std::string port);

    /**
     * @brief Disconnect from the PostgreSQL database.
     */
    void disconnect();

    /**
     * @brief Fetch all blocks from the database.
     * @return JSON object containing information about all blocks.
     */
    std::optional<nlohmann::json> fetchAllBlocks();

    /**
     * @brief Fetch all transactions from the database.
     * @return JSON object containing information about all transactions.
     */
    json fetchAllTransactions();

    /**
     * @brief Fetch paginated transactions from the database.
     * @param page Page number.
     * @param limit Number of transactions per page.
     * @param isReversed Flag indicating whether to fetch transactions in reverse order.
     * @return JSON object containing paginated transactions.
     */
    std::optional<json> fetchPaginatedTransactions(int page, int limit, bool isReversed);

    /**
     * @brief Fetch paginated blocks from the database.
     * @param page Page number.
     * @param limit Number of blocks per page.
     * @param reverseOrder Flag indicating whether to fetch blocks in reverse order.
     * @return JSON object containing paginated blocks.
     */
    std::optional<json> fetchPaginatedBlocks(int page, int limit, bool reverseOrder);

    /**
     * @brief Fetch the total transaction count from the database.
     * @return Total number of transactions as an optional uint64_t.
     */
    std::optional<uint64_t> fetchTotalTransactionCount();

    /**
     * @brief Fetch the total block count from the database.
     * @return Total number of blocks as an optional uint64_t.
     */
    std::optional<uint64_t> fetchTotalBlocksCount();

    /**
     * @brief Fetch a block by its hash from the database.
     * @param block_hash Hash of the block to fetch.
     * @return JSON object containing information about the specified block.
     */
    std::optional<json> fetchBlockByHash(const std::string &block_hash);

    /**
     * @brief Fetch a transaction by its hash from the database.
     * @param transaction_hash Hash of the transaction to fetch.
     * @return JSON object containing information about the specified transaction.
     */
    std::optional<json> fetchTransactionByHash(const std::string &transaction_hash);

    /**
     * @brief Fetch details of transactions from a list of transaction IDs.
     * @param transaction_ids Vector of transaction IDs to fetch details for.
     * @return JSON object containing information about the specified transactions.
     */
    std::optional<json> fetchTransactionsDetailsFromIds(const std::vector<std::string> &transaction_ids);

    /**
     * @brief Fetch transparent inputs related to a transaction ID.
     * @param transaction_id ID of the transaction to fetch transparent inputs for.
     * @return JSON object containing information about transparent inputs.
     */
    std::optional<json> fetchTransparentInputsRelatedToTransactionId(const std::string &transaction_id);

    /**
     * @brief Fetch transparent outputs related to a transaction ID.
     * @param transaction_id ID of the transaction to fetch transparent outputs for.
     * @return JSON object containing information about transparent outputs.
     */
    std::optional<json> fetchTransparentOutputsRelatedToTransactionId(const std::string &transaction_id);

    /**
     * @brief Fetch information about connected peers.
     * @return JSON object containing information about connected peers.
     */
    std::optional<json> fetchPeerInfo();

    /**
     * @brief Fetch information about the blockchain.
     * @return JSON object containing information about the blockchain.
     */
    std::optional<json> fetchBlockchainInfo();

    /**
     * @brief Fetch transactions within the last X days based on timestamps.
     * @param startTimestamp Start timestamp for the X-day period.
     * @param endTimestamp End timestamp for the X-day period.
     * @return Vector of JSON objects containing information about transactions within the specified time range.
     */
    std::optional<json> fetchTransactionInPeriod(uint64_t startTimestamp, uint64_t endTimestamp);

    /**
     * @brief Create a JSON error response for exception handling.
     * @param e Exception object.
     */
    void createJsonErrorResponse(json &errorResponse, const std::exception &e);

    std::optional<uint64_t> getMostRecentTransactionTimestamp();

    std::optional<json> directSearch(const std::string &);

private:
    bool is_connected;                                            ///< Flag indicating whether the database is connected.
    std::queue<std::unique_ptr<pqxx::connection>> connectionPool; ///< Connection pool for managing database connections.
    std::mutex cs_pool_mutex;
    const std::string prepared_direct_search_statement = "direct_search_query"; ///< Mutex for thread-safe access to the connection pool.

    /**
     * @brief Shutdown all database connections.
     */
    void ShutdownConnections();

    /**
     * @brief Release a database connection back to the pool.
     * @param conn Database connection to release.
     * @return True if the connection was successfully released, false otherwise.
     */
    bool ReleaseConnection(std::unique_ptr<pqxx::connection> conn);

    /**
     * @brief Get a database connection from the pool.
     * @return A unique pointer to a database connection.
     */
    std::unique_ptr<pqxx::connection> GetConnection();

    /**
     * @brief Convert a Unix timestamp to a date string. ( "YYYY-MM-DD" )
     * @param timestamp Unix timestamp to convert.
     * @return Date string in a specific format.
     */
    std::string unixTimestampToDateString(uint64_t timestamp);
};

struct ManagedConnection
{
public:
    ManagedConnection(Database &db_) : db(db_), conn(db_.GetConnection()) {}
    ~ManagedConnection()
    {
        db.ReleaseConnection(std::move(conn));
    }

    pqxx::connection& operator*() {
        return *conn;
    }

    pqxx::connection* operator->() {
        return conn.get();
    }

private:
    Database &db;
    std::unique_ptr<pqxx::connection> conn;
};