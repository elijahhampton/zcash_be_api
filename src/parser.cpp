#include "parser.hpp"

#ifndef PARSER_CPP
#define PARSER_CPP

/**
 * Converts postgres rows to JSON
*/
json Parser::row_to_json(const pqxx::row& r) {
    json j;
    for (const auto& field : r) {
        j[field.name()] = field.c_str();
    }
    return j;
}

bool Parser::StringToBool(const std::string& str) {
    // Convert the string to lowercase to make the comparison case-insensitive
    std::string lowerCaseStr = str;
    std::transform(lowerCaseStr.begin(), lowerCaseStr.end(), lowerCaseStr.begin(), ::tolower);

    // "true" or "1" will be considered true, anything else will be false
    return lowerCaseStr == "true" || lowerCaseStr == "1";
}

#endif //PARSER_CPP