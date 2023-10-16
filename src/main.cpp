#include "routes.hpp"
#include "../include/crow_all.h"

int main(int argc, char* argv[]) {

        // Check the number of arguments injected
    if (argc < 5)
    {
        throw std::runtime_error("Invalid number of arguments. Arguments must include: --dbname, --dbpassword, --dbuser, --dbhost, --dbport, --rpcusername, --rpcpassword, --rpcurl");
    }

    std::string db_name{""};
    std::string db_password{""};
    std::string db_user{""};
    std::string db_host{""};
    std::string db_port{""};

    //  Check for the required arguments and define the InitConfig fields
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--dbname" && i + 1 < argc)
        {
            db_name = argv[++i];
        }
        else if (arg == "--dbpassword" && i + 1 < argc)
        {
            db_password = argv[++i];
        }
        else if (arg == "--dbuser" && i + 1 < argc)
        {
            db_user = argv[++i];
        }
        else if (arg == "--dbhost" && i + 1 < argc)
        {
            db_host = argv[++i];
        }
        else if (arg == "--dbport" && i + 1 < argc)
        {
            db_port = argv[++i];
        }
        else
        {
            std::cerr << "Unknown or improperly used argument: " << arg << std::endl;
            return 1;
        }
    }

    Database database;

    ZCashApi api(database);
    crow::SimpleApp app;
    api.init(app, db_name, db_user, db_password, db_host, db_port);
    

    app.port(4000).multithreaded().run();
    return 0;
}