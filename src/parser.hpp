#include <pqxx/pqxx>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Parser {
    public:
        static json row_to_json(const pqxx::row& r);
};