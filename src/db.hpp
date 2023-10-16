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

        void connect(const std::string& dbname, const std::string& user, const std::string& password, const std::string& host, uint8_t port);
        void disconnect();

        std::optional<nlohmann::json> fetchAllBlocks();
        json fetchAllTransactions();
        json fetchTransactionsSinceHeight(uint32_t lastBlockHeight);
        json fetchBlocksSinceHeight(uint32_t lastBlockHeight);
    
    private:
        std::unique_ptr<pqxx::connection> conn;
        bool is_connected;

};