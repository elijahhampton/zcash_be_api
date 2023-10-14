#include <string>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <memory>
#include <fstream>

using json = nlohmann::json;

class Database {
    public:
        Database();
        ~Database();

        void connect();
        void disconnect();

        std::optional<nlohmann::json> fetchAllBlocks();
        json fetchAllTransactions();
        json fetchTransactionsSinceHeight(uint32_t lastBlockHeight);
        json fetchBlocksSinceHeight(uint32_t lastBlockHeight);
    
    private:
        std::unique_ptr<pqxx::connection> conn;
        bool is_connected;

        std::string loadConfig(const std::string& path);
};