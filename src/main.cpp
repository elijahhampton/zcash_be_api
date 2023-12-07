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

        uint16_t api_port = std::stoi(Config::getApiPort());
        api.init(app, Config::getDatabaseName(), Config::getDatabaseUser(), Config::getDatabasePassword(), Config::getDatabaseHost(), Config::getDatabasePort());
        app.loglevel(crow::LogLevel::DEBUG);

        // Run the server asynchronously
        auto server_future = std::async(std::launch::async, [&]() {
            app.port(api_port).multithreaded().run();
        });

        server_future.get();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}