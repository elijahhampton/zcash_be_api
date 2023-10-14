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

#endif //PARSER_CPP