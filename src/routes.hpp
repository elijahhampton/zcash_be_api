#include "../include/crow_all.h"
#include "db.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class ZCashApi
{
public:
    /**
     * @brief Constructor for ZCashApi.
     * @param database Reference to the Database instance for handling Zcash data.
     */
    ZCashApi(Database &database);

    /**
     * @brief Destructor for ZCashApi.
     */
    ~ZCashApi() noexcept = default;

    /**
     * @brief Initialize the ZCashApi by configuring routes and connecting to the specified database.
     * @param app Crow application instance to configure routes.
     * @param dbname Database name.
     * @param user Database user name.
     * @param password Database user password.
     * @param host Database host.
     * @param port Database port.
     */
    void init(crow::App<crow::CORSHandler> &app, const std::string &dbname, const std::string &user, const std::string &password, const std::string &host, std::string port);

    /**
     * @brief Handle the "hello" route, responding with a simple greeting.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void hello_route(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching all blocks.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_all_blocks_route(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching all transactions.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_all_transactions_route(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching paginated transactions.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_paginated_transactions_route(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching paginated blocks.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_paginated_blocks_route(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching a transaction by its hash.
     * @param req Crow request object.
     * @param res Crow response object.
     * @param transaction_hash Hash of the transaction to fetch.
     */
    void fetch_transaction_by_hash(const crow::request &req, crow::response &res, const std::string &transaction_hash);

    /**
     * @brief Handle the route for fetching a block by its hash.
     * @param req Crow request object.
     * @param res Crow response object.
     * @param block_hash Hash of the block to fetch.
     */
    void fetch_block_by_hash(const crow::request &req, crow::response &res, const std::string &block_hash);

    /**
     * @brief Handle the route for fetching transparent outputs related to a transaction hash.
     * @param req Crow request object.
     * @param res Crow response object.
     * @param transaction_hash Hash of the transaction to fetch transparent outputs for.
     */
    void fetch_transparent_outputs_related_to_transaction_hash(const crow::request &req, crow::response &res, const std::string &transaction_hash);

    /**
     * @brief Handle the route for fetching transparent inputs related to a transaction hash.
     * @param req Crow request object.
     * @param res Crow response object.
     * @param transaction_hash Hash of the transaction to fetch transparent inputs for.
     */
    void fetch_transparent_inputs_related_to_transaction_hash(const crow::request &req, crow::response &res, const std::string &transaction_hash);

    /**
     * @brief Handle the route for fetching details of transactions from a list of transaction IDs.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_transactions_details_from_ids(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching information about connected peers.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_peer_info(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching information about the blockchain.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_blockchain_info(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching the total transaction count.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_total_transaction_count(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching the total block count.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_total_block_count(const crow::request &req, crow::response &res);

    /**
     * @brief Handle the route for fetching the total transaction counts within a specified period.
     * @param req Crow request object.
     * @param res Crow response object.
     */
    void fetch_total_transaction_counts_in_period(const crow::request &req, crow::response &res);

    void direct_search(const crow::request &req, crow::response &res);
private:
    /**
     * @brief Reference to the Database instance for handling Zcash data.
     */
    Database &db;

    /**
     * @brief Flag indicating whether the ZCashApi has been initiated.
     */
    bool isInitiated{false};

    /**
     * @brief Set up the HTTP routes for the ZCashApi.
     * @param app Crow application instance to configure routes.
     */
    void setup_routes(crow::App<crow::CORSHandler> &app);

    /**
     * @brief Set common HTTP headers for a Crow response.
     * @param res Crow response object to set headers for.
     */
    void set_common_headers(crow::response &res);
};
