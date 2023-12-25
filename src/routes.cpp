#ifndef ROUTES_HPP
#define ROUTES_HPP

#include "routes.hpp"
#include "parser.hpp"
#include "../include/crow_all.h"
#include <optional>

ZCashApi::ZCashApi(Database &database) : db(database) {}

/**
 * Initialize the API. This method makes a call to also initialize the database
 * and setup routes.
 */
void ZCashApi::init(crow::App<crow::CORSHandler> &app, const std::string &dbname, const std::string &user, const std::string &password, const std::string &host, std::string port)
{
    if (this->isInitiated)
    {
        throw std::runtime_error("API is already initialized.");
    }

    this->isInitiated = true;
    db.connect(dbname, user, password, host, port);
    this->setup_routes(app);
}

// Hello World
void ZCashApi::hello_route(const crow::request &req, crow::response &res)
{
    res.code = 200;
    res.write("Welcome to the zcash api.");
}

/**
 * Fetch all blocks from the database.
 */
void ZCashApi::fetch_all_blocks_route(const crow::request &req, crow::response &res)
{
    try
    {
        std::optional<json> result = db.fetchAllBlocks();

        if (!result.has_value())
        {
            res.code = 404;
            res.write({});
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (const std::exception &e)
    {
        res.code = 500;
        res.write(json());
        CROW_LOG_CRITICAL << e.what();
    }
}

/**
 * Fetch all transactions from the database.
 */
void ZCashApi::fetch_all_transactions_route(const crow::request &req, crow::response &res)
{
    try
    {
        std::optional<json> result = db.fetchAllTransactions();

        if (!result.has_value())
        {
            res.code = 404;
            res.write({});
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;

    }
}

// Fetch blocks with pagination
void ZCashApi::fetch_paginated_blocks_route(const crow::request &req, crow::response &res)
{
    int page = std::stoi(req.url_params.get("page") ? req.url_params.get("page") : "1");
    int limit = std::stoi(req.url_params.get("limit") ? req.url_params.get("limit") : "50");
    bool isReversed = req.url_params.get("reversedOrder") ? Parser::StringToBool(req.url_params.get("reversedOrder")) : false;

    try
    {
        std::optional<json> result = db.fetchPaginatedBlocks(page, limit, isReversed);

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 400;
            return;
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

// Fetch transactions with pagination
void ZCashApi::fetch_paginated_transactions_route(const crow::request &req, crow::response &res)
{
    uint64_t page = std::stoi(req.url_params.get("page") ? req.url_params.get("page") : "1");
    uint64_t limit = std::stoi(req.url_params.get("limit") ? req.url_params.get("limit") : "50");
    bool isReversed = req.url_params.get("reversedOrder") ? Parser::StringToBool(req.url_params.get("reversedOrder")) : false;

    try
    {
        std::optional<json> result = db.fetchPaginatedTransactions(page, limit, isReversed);

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 400;
            return;
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_block_by_hash(const crow::request &req, crow::response &res, const std::string &block_hash)
{
    try
    {
        std::optional<json> result = db.fetchBlockByHash(block_hash);

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 400;
            return;
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_transaction_by_hash(const crow::request &req, crow::response &res, const std::string &transaction_hash)
{
    try
    {
        std::optional<json> result = db.fetchTransactionByHash(transaction_hash);

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 400;
            return;
        }

        json transaction = result.value();
        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_transparent_outputs_related_to_transaction_hash(const crow::request &req, crow::response &res, const std::string &transaction_hash)
{
    try
    {
        std::optional<json> result = db.fetchTransparentOutputsRelatedToTransactionId(transaction_hash);

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 400;
            return;
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_transparent_inputs_related_to_transaction_hash(const crow::request &req, crow::response &res, const std::string &transaction_hash)
{
    try
    {
        std::optional<json> result = db.fetchTransparentInputsRelatedToTransactionId(transaction_hash);

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 400;
            return;
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_transactions_details_from_ids(const crow::request &req, crow::response &res)
{
    try
    {
        std::vector<std::string> transaction_ids = json::parse(req.body);
        std::optional<json> result = db.fetchTransactionsDetailsFromIds(transaction_ids);

        if (transaction_ids.empty())
        {
            res.write(json({}));
            res.code = 200;
            return;
        }

        if (!result.has_value())
        {
            throw;
        }

        res.code = 200;
        res.write(result.value().dump());
    }
    catch (std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_peer_info(const crow::request &req, crow::response &res)
{
    try
    {
        std::optional<json> result = db.fetchPeerInfo();

        if (!result.has_value())
        {
            res.write(json({}));
            res.code = 404;
            return;
        }

        res.write(result.value().dump());
        res.code = 200;
    }
    catch (const std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_blockchain_info(const crow::request &req, crow::response &res)
{
    try
    {
        std::optional<json> result = db.fetchBlockchainInfo();
        if (!result.has_value())
        {
            res.write(0);
            res.code = 404;
            return;
        }

        res.write(result.value().dump());
        res.code = 200;
    }
    catch (const std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_total_block_count(const crow::request &req, crow::response &res)
{
    try
    {
        std::optional<json> result = db.fetchTotalBlocksCount();

        if (!result.has_value())
        {
            res.write(0);
            res.code = 404;
            return;
        }

        res.write(result.value().dump());
        res.code = 200;
    }
    catch (const std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_total_transaction_count(const crow::request &req, crow::response &res)
{
    try
    {
        std::optional<json> result = db.fetchTotalTransactionCount();
        if (!result.has_value())
        {
            json jsonResponse;
            jsonResponse["error"] = "Unable to obtain transaction count.";
            res.write(jsonResponse.dump());
            res.code = 404;
            return;
        }

        res.write(json(result.value()).dump());
        res.code = 200;
    }
    catch (const std::exception &e)
    {
                CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::fetch_total_transaction_counts_in_period(const crow::request &req, crow::response &res)
{
    try
    {
        // Fetch the most recent timestamp from the transactions table
        std::optional<uint64_t> mostRecentTimestampOpt = db.getMostRecentTransactionTimestamp();

        if (!mostRecentTimestampOpt.has_value()) {
            json jsonResponse;
            jsonResponse["error"] = "No transactions found in the database.";
            res.write(jsonResponse.dump());
            res.code = 404;
            return;
        }

        uint64_t mostRecentTimestamp = mostRecentTimestampOpt.value();

        // Calculate the start timestamp (14 days before the most recent timestamp)
        auto mostRecentDate = std::chrono::system_clock::from_time_t(mostRecentTimestamp);
        auto startDate = mostRecentDate - std::chrono::hours(24 * 14);
        uint64_t startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(startDate.time_since_epoch()).count();

        CROW_LOG_DEBUG << "Start timestamp " << startTimestamp;

        // Fetch transaction data
        std::optional<json> result = db.fetchTransactionInPeriod(startTimestamp, mostRecentTimestamp);
        
        if (!result.has_value())
        {
            json jsonResponse;
            jsonResponse["error"] = "Unable to obtain transaction count.";
            res.write(jsonResponse.dump());
            res.code = 404;
            return;
        }

        res.write(result.value().dump());
        res.code = 200;
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}

void ZCashApi::direct_search(const crow::request &req, crow::response &res)
{
    try {
        const std::string searchPattern = req.url_params.get("pattern");

        std::optional<json> searchOptVal = this->db.directSearch(searchPattern);
        if (!searchOptVal.has_value()) {
            res.code = 404;
            return;
        }

        res.code = 200;
        res.write(searchOptVal.value());

    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        CROW_LOG_CRITICAL << e.what();
        json errorResponse;
        this->db.createJsonErrorResponse(errorResponse, e);
        res.write(errorResponse.dump());
        res.code = 500;
    }
}



// Private

/**
 * Adds common headers to response objects.
 */
void ZCashApi::set_common_headers(crow::response &res)
{

    res.set_header("Content-Type", "application/json");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Accept");
    res.set_header("Access-Control-Allow-Origin", "*");
}

/**
 * Sets up API routes.
 */
void ZCashApi::setup_routes(crow::App<crow::CORSHandler> &app)
{
    CROW_ROUTE(app, "/hello").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                             {
        this->set_common_headers(res);
        this->hello_route(req, res);
        res.end(); });

    CROW_ROUTE(app, "/blocks/all").methods(crow::HTTPMethod::OPTIONS)([this](const crow::request &req)
                                                                      {
        CROW_LOG_INFO << "OPTIONS blocks/all";
        return crow::response(crow::status::OK); });

    CROW_ROUTE(app, "/block/<string>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res, const std::string &block_hash)
                                                                      {
        this->set_common_headers(res);
        this->fetch_block_by_hash(req, res, block_hash);
        res.end(); });

    CROW_ROUTE(app, "/transaction/<string>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res, const std::string &transaction_hash)
                                                                            {
        this->set_common_headers(res);
        this->fetch_transaction_by_hash(req, res, transaction_hash);
        res.end(); });

    CROW_ROUTE(app, "/blocks/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                  {
        this->set_common_headers(res);
        this->fetch_all_blocks_route(req, res);
        res.end(); });

    CROW_ROUTE(app, "/transactions/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                        {
        this->set_common_headers(res);
        this->fetch_all_transactions_route(req, res);
        res.end(); });

    CROW_ROUTE(app, "/blocks").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                              {
        this->set_common_headers(res);
        this->fetch_paginated_blocks_route(req, res);
        res.end(); });

    CROW_ROUTE(app, "/transactions").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                    {
        this->set_common_headers(res);
        this->fetch_paginated_transactions_route(req, res);
        res.end(); });

    CROW_ROUTE(app, "/transaction/outputs/<string>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res, const std::string &transaction_hash)
                                                                                    {
        this->set_common_headers(res);
        this->fetch_transparent_outputs_related_to_transaction_hash(req, res, transaction_hash);
        res.end(); });

    CROW_ROUTE(app, "/transaction/inputs/<string>").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res, const std::string &transaction_hash)
                                                                                   {
        this->set_common_headers(res);
        this->fetch_transparent_inputs_related_to_transaction_hash(req, res, transaction_hash);
        res.end(); });

    // has_body
    CROW_ROUTE(app, "/transactions/details").methods(crow::HTTPMethod::POST)([this](const crow::request &req, crow::response &res)
                                                                             {
        this->set_common_headers(res);
        this->fetch_transactions_details_from_ids(req, res);
        res.end(); });

    CROW_ROUTE(app, "/peers/details").methods(crow::HTTPMethod::POST)([this](const crow::request &req, crow::response &res)
                                                                      {
        this->set_common_headers(res);
        this->fetch_peer_info(req, res);
        res.end(); });

    CROW_ROUTE(app, "/chain").methods(crow::HTTPMethod::POST)([this](const crow::request &req, crow::response &res)
                                                              {
        this->set_common_headers(res);
        this->fetch_blockchain_info(req, res);
        res.end(); });

    CROW_ROUTE(app, "/transactions/total").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                          {
        this->set_common_headers(res);
        this->fetch_total_transaction_count(req, res);
        res.end(); });

    CROW_ROUTE(app, "/blocks/total").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                    {
        this->set_common_headers(res);
        this->fetch_total_block_count(req, res);
        res.end(); });

    CROW_ROUTE(app, "/metrics/transactions").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                            {
        this->set_common_headers(res);
        this->fetch_total_transaction_counts_in_period(req, res);
        res.end(); });

    CROW_ROUTE(app, "/search").methods(crow::HTTPMethod::POST)([this](const crow::request &req, crow::response &res)
                                                                            {
        this->set_common_headers(res);
        this->direct_search(req, res);
        res.end(); });
}

#endif // ROUTES_HPP