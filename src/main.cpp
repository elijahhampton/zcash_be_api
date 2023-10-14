#include "routes.hpp"
#include "../include/crow_all.h"

int main() {

    Database database;
    database.connect();

    ZCashApi api(database);
    crow::SimpleApp app;
    api.init(app);
    

    app.port(4000).multithreaded().run();
    return 0;
}