#include "routes.hpp"
#include "../include/crow_all.h"
#include "config.h"

int main() {
    Database database;
    ZCashApi api(database);
    crow::App<crow::CORSHandler> app;

    try {
        auto& cors = app.get_middleware<crow::CORSHandler>();

        cors.global()
        .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("POST"_method, "GET"_method)
      .prefix("/cors")
        .origin("ec2-18-222-152-234.us-east-2.compute.amazonaws.com")
      .prefix("/nocors")
        .ignore();
        
        api.init(app, Config::getDatabaseName(), Config::getDatabaseUser(), Config::getDatabasePassword(), Config::getDatabaseHost(), Config::getDatabasePort());
        app.port(8000).multithreaded().run();
    } catch(const std::exception& e) {
        std::cout << "Trouble starting API: " << std::endl;
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}