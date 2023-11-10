#ifndef ROUTES_HPP
#define ROUTES_HPP

#include "routes.hpp"
#include "../include/crow_all.h"
#include <optional>

// Public

// Constructor
ZCashApi::ZCashApi(Database &database) : db(database) {}

// Destructor
ZCashApi::~ZCashApi() {}

/**
 * Initialize the API. This method makes a call to also initialize the database
 * and setup routes.
 */
void ZCashApi::init(crow::SimpleApp &app, const std::string &dbname, const std::string &user, const std::string &password, const std::string &host, std::string port)
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
    res.write("Hello World");
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
            throw std::runtime_error("fetch_all_blocks_route: NULL");
        }

        json allBlocks = result.value();
        res.code = 200;
        res.write(allBlocks.dump());
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        res.code = 500;
        res.write(json(nullptr));
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
            throw std::runtime_error("fetch_all_transactions_route: NULL");
        }

        json allTransactions = result.value();
        res.code = 200;
        res.write(allTransactions.dump());
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        res.code = 500;
        res.write(json(nullptr));
    }
}

// Fetch blocks with pagination
void ZCashApi::fetch_paginated_blocks_route(const crow::request &req, crow::response &res)
{
    int page = std::stoi(req.url_params.get("page") ? req.url_params.get("page") : "1");
    int limit = std::stoi(req.url_params.get("limit") ? req.url_params.get("limit") : "50");

    try
    {
        std::optional<json> result = db.fetchPaginatedBlocks(page, limit);

        if (!result.has_value())
        {
            throw std::runtime_error("fetch_paginated_blocks_route: NULL");
        }

        json blocks = result.value();
        std::cout << blocks << std::endl;
        res.code = 200;
        res.write(blocks.dump());
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        res.code = 500;
        res.write(json(nullptr));
    }
}

// Fetch transactions with pagination
void ZCashApi::fetch_paginated_transactions_route(const crow::request &req, crow::response &res)
{
    int page = std::stoi(req.url_params.get("page") ? req.url_params.get("page") : "1");
    int limit = std::stoi(req.url_params.get("limit") ? req.url_params.get("limit") : "50");

    try
    {
        std::optional<json> result = db.fetchPaginatedTransactions(page, limit);

        if (!result.has_value())
        {
            throw std::runtime_error("fetch_paginated_transactions_route: NULL");
        }

        json transactions = result.value();
        std::cout << transactions << std::endl;
        res.code = 200;
        res.write(transactions.dump());
    }
    catch (std::exception &e)
    {
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
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Accept");
    res.set_header("Access-Control-Max-Age", "3600");
}

/**
 * Sets up API routes.
 */
void ZCashApi::setup_routes(crow::SimpleApp &app)
{
    CROW_ROUTE(app, "/hello").methods(crow::HTTPMethod::GET)([this](const crow::request &req)
                                                             {
        crow::response res;
        this->set_common_headers(res);
        this->hello_route(req, res);
        res.end();
        return res; });

    CROW_ROUTE(app, "/blocks/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req)
                                                                  {
        crow::response res;
        this->set_common_headers(res);
        this->fetch_all_blocks_route(req, res);
        res.end();
        return res; });

    CROW_ROUTE(app, "/transactions/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req)
                                                                        {
        crow::response res;
        this->set_common_headers(res);
        this->fetch_all_transactions_route(req, res);
        res.end();
        return res; });

    CROW_ROUTE(app, "/blocks").methods(crow::HTTPMethod::GET)([this](const crow::request &req)
                                                              {
        crow::response res;
        this->set_common_headers(res);
        this->fetch_paginated_blocks_route(req, res);
        res.end();
        return res; });

    // Route for paginated transactions
    CROW_ROUTE(app, "/transactions").methods(crow::HTTPMethod::GET)([this](const crow::request &req)
                                                                    {
        crow::response res;
        this->set_common_headers(res);
        this->fetch_paginated_transactions_route(req, res);
        res.end();
        return res; });
}

#endif // ROUTES_HPP