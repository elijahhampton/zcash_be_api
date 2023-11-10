#include "../include/crow_all.h"
#include "db.hpp"

class ZCashApi {
    public:
        ZCashApi(Database& database);
        ~ZCashApi();
        void init(crow::App<crow::CORSHandler> &app, const std::string& dbname, const std::string& user, const std::string& password, const std::string& host, std::string port);
        void hello_route(const crow::request& req, crow::response& res);
        void fetch_all_blocks_route(const crow::request& req, crow::response& res);
        void fetch_all_transactions_route(const crow::request& req, crow::response& res);
        void fetch_paginated_transactions_route(const crow::request &req, crow::response &res);
        void fetch_paginated_blocks_route(const crow::request &req, crow::response &res);

    private:
        Database &db;
        bool isInitiated{false};
        void setup_routes(crow::App<crow::CORSHandler>& app);
        void set_common_headers(crow::response& res);
};

