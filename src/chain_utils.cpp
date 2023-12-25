#include "chain_utils.hpp"

bool ChainUtils::isValidSHA256Hash(const std::string& hash) noexcept {
    size_t hashLength = hash.size();
    return hashLength == ZCASH_HEXADECIMAL_HASH_LENGTH;

    return false;
}