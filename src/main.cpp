#include "routes.hpp"
#include "../include/crow_all.h"
#include "config.h"
#include <sstream>

int main()
{
    Database database;
    ZCashApi api(database);
    crow::App<crow::CORSHandler> app;

    try
    {
        uint16_t api_port = std::stoi(Config::getApiPort());
        api.init(app, Config::getDatabaseName(), Config::getDatabaseUser(), Config::getDatabasePassword(), Config::getDatabaseHost(), Config::getDatabasePort());
        app.loglevel(crow::LogLevel::DEBUG);
        app.port(api_port).multithreaded().run();
    }
    catch (const std::exception &e)
    {
        std::cout << "AAA" << std::endl;
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}