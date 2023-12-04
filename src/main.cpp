#include "routes.hpp"
#include "../include/crow_all.h"
#include "config.h"

int main()
{
    Database database;
    ZCashApi api(database);
    crow::App<crow::CORSHandler> app;

    try
    {
        auto &cors = app.get_middleware<crow::CORSHandler>();

        cors.global()
            .methods("POST"_method, "GET"_method)
            .prefix("/")
            .origin("ec2-18-222-152-234.us-east-2.compute.amazonaws.com")
            .allow_credentials();

        uint16_t api_port = std::stoi(Config::getApiPort());
        api.init(app, Config::getDatabaseName(), Config::getDatabaseUser(), Config::getDatabasePassword(), Config::getDatabaseHost(), Config::getDatabasePort());
        app.port(api_port).multithreaded().run();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}