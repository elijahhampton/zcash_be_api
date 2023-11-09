#include "routes.hpp"
#include "../include/crow_all.h"
#include "config.h"

int main() {
    Database database;
    ZCashApi api(database);
    crow::SimpleApp app;
    try {
        api.init(app, Config::getDatabaseName(), Config::getDatabaseUser(), Config::getDatabasePassword(), Config::getDatabaseHost(), Config::getDatabasePort());
        app.port(8000).multithreaded().run();
    } catch(const std::exception& e) {
        std::cout << "Trouble starting API: " << std::endl;
        std::cout << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}