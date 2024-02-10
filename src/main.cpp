#include "routes.hpp"
#include "../include/crow_all.h"
#include "config.h"
#include <sstream>
#include <future>

int main() {
    try {
        Database database;
        ZCashApi api(database);
        crow::App<crow::CORSHandler> app;

        // Establish api port
        uint16_t api_port = std::stoi(Config::getApiPort());

        // Init API
        api.init(app, Config::getDatabaseName(), Config::getDatabaseUser(), Config::getDatabasePassword(), Config::getDatabaseHost(), Config::getDatabasePort());
        
        // Set log level
        app.loglevel(crow::LogLevel::DEBUG);

        // Run api with multithreading
        app.port(api_port).multithreaded().run();

    } catch (const std::exception &e) {
        return 1;
    }

    return 0;
}