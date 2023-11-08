#include <string>
#include <pqxx/pqxx>
#include <mutex>
#include <queue>
#include "nlohmann/json.hpp"
#include <memory>
#include <fstream>

using json = nlohmann::json;

class Database {
    public:
        Database();
        ~Database();

        void connect(const std::string& dbname, const std::string& user, const std::string& password, const std::string& host, std::string port);
        void disconnect();

        std::optional<nlohmann::json> fetchAllBlocks();
        json fetchAllTransactions();
        std::optional<json> fetchPaginatedTransactions(int page, int limit);
        std::optional<json> fetchPaginatedBlocks(int page, int limit);
    
    private:
        bool is_connected;
        std::queue<std::unique_ptr<pqxx::connection>> connectionPool;
        std::mutex poolMutex;
            void ShutdownConnections();
    bool ReleaseConnection(std::unique_ptr<pqxx::connection> conn);
    std::unique_ptr<pqxx::connection> GetConnection();

};