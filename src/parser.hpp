#include "nlohmann/json.hpp"
#include <pqxx/pqxx>

using json = nlohmann::json;

/**
 * @brief The Parser class provides static utility methods for converting data types and parsing values.
 */
class Parser {
public:
    Parser() = default;
    ~Parser() noexcept = default;
    
    /**
     * @brief Convert a PostgreSQL row to a JSON object.
     * @param r PostgreSQL row to convert.
     * @return JSON object representing the contents of the PostgreSQL row.
     */
    static json row_to_json(const pqxx::row& r);

    /**
     * @brief Convert a string to a boolean value.
     * @param str String to convert.
     * @return Boolean value representing the parsed result. Returns false if the conversion fails.
     */
    static bool StringToBool(const std::string& str);
};
