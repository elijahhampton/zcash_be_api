#include <pqxx/pqxx>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Parser {
    public:
        static json row_to_json(const pqxx::row& r);
        static bool StringToBool(const std::string& str);
};