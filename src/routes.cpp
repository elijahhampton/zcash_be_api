#ifndef ROUTES_HPP
#define ROUTES_HPP

#include "routes.hpp"
#include "../include/crow_all.h"
#include <optional>

// Public

// Constructor
ZCashApi::ZCashApi(Database& database): db(database) {}

// Destructor
ZCashApi::~ZCashApi() {}

/**
 * Initialize the API. This method makes a call to also initialize the database
 * and setup routes.
*/
void ZCashApi::init(crow::SimpleApp &app)
{
    if (this->isInitiated)
    {
        throw std::runtime_error("API is already initialized.");
    }

    this->isInitiated = true;
    db.connect();
    this->setup_routes(app);
}

// Hello World
void ZCashApi::hello_route(const crow::request &req, crow::response &res)
{
    res.code = 200;
    res.write("Hello World");
}

/**
 * Fetch all blocks from the database.
*/
void ZCashApi::fetch_all_blocks_route(const crow::request &req, crow::response &res)
{
    try {
        std::optional<json> result = db.fetchAllBlocks();

        if (!result.has_value()) {
            throw std::runtime_error("fetch_all_blocks_route: NULL");
        }

        json allBlocks = result.value();
        res.code = 200;
        res.write(allBlocks.dump());
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        res.code = 500;
        res.write(json(nullptr));
    }
}

/**
 * Fetch all transactions from the database.
*/
void ZCashApi::fetch_all_transactions_route(const crow::request &req, crow::response &res) {
    try {
        std::optional<json> result = db.fetchAllTransactions();

        if (!result.has_value()) {
            throw std::runtime_error("fetch_all_transactions_route: NULL");
        }

        json allTransactions = result.value();
        res.code = 200;
        res.write(allTransactions.dump());
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        res.code = 500;
        res.write(json(nullptr));
    }
}

// Private

/**
 * Adds common headers to response objects.
*/
void ZCashApi::set_common_headers(crow::response &res)
{
    res.set_header("Content-Type", "application/json");
         // Set CORS headers
        res.set_header("Access-Control-Allow-Origin", "*"); // Allow all origins (Change * to specific origin if needed)
        res.set_header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS"); // Add other methods as needed
};

/**
 * Sets up API routes.
*/
void ZCashApi::setup_routes(crow::SimpleApp &app)
{
    CROW_ROUTE(app, "/hello").methods(crow::HTTPMethod::GET)([this](const crow::request &req) {
        crow::response res;
        this->set_common_headers(res);
        this->hello_route(req, res);
        return res; 
    });

    CROW_ROUTE(app, "/blocks/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req) {
        crow::response res;
        this->set_common_headers(res);
        this->fetch_all_blocks_route(req, res);
        return res; 
    });

    CROW_ROUTE(app, "/transactions/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req) {
        crow::response res;
        this->set_common_headers(res);
        this->fetch_all_transactions_route(req, res);
        return res; 
    });
}

#endif // ROUTES_HPP