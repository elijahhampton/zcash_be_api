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
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Accept");
    res.set_header("Access-Control-Allow-Origin", "*");
}

/**
 * Sets up API routes.
 */
void ZCashApi::setup_routes(crow::App<crow::CORSHandler> &app)
{
    CROW_ROUTE(app, "/hello").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res) {
        this->set_common_headers(res);
        this->hello_route(req, res);
        res.end();
    });

    CROW_ROUTE(app, "/blocks/all").methods(crow::HTTPMethod::OPTIONS)([this](const crow::request &req){
        CROW_LOG_INFO << "OPTIONS blocks/all";
        return crow::response(crow::status::OK);
    });

    CROW_ROUTE(app, "/blocks/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                  {
        this->set_common_headers(res);
        this->fetch_all_blocks_route(req, res);
        res.end();
        });

    CROW_ROUTE(app, "/transactions/all").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                        {
        this->set_common_headers(res);
        this->fetch_all_transactions_route(req, res);
        res.end();
        });

    CROW_ROUTE(app, "/blocks").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                              {
        this->set_common_headers(res);
        this->fetch_paginated_blocks_route(req, res);
        res.end();
         });

    // Route for paginated transactions
    CROW_ROUTE(app, "/transactions").methods(crow::HTTPMethod::GET)([this](const crow::request &req, crow::response &res)
                                                                    {
        this->set_common_headers(res);
        this->fetch_paginated_transactions_route(req, res);
        res.end();
        });

    CROW_ROUTE(app, "/.well-known/pki-validation/<string>").methods(crow::HTTPMethod::GET)([this](const crow::request& req, std::string filename){
        // Construct the full path to the file on the server
        std::string full_path = "~/" + filename;

        // Read the file from disk and return it as a response
        std::ifstream file(full_path, std::ios::binary | std::ios::ate);
        if(file) {
            auto length = file.tellg();
            file.seekg(0, std::ios::beg);

            std::string content(length, '\0');
            file.read(&content[0], length);

            crow::response resp(content);
            this->set_common_headers(resp);
            resp.set_header("Content-Type", "text/plain");
            return resp;
        } else {
            return crow::response(404);
        }
    });

}

#endif // ROUTES_HPP