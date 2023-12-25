#ifndef CHAIN_UTILS
#define CHAIN_UTILS

#include <string>

const uint64_t ZCASH_HEXADECIMAL_HASH_LENGTH = 64;
const uint64_t ZCASH_SHA256_HASH_LENGTH_IN_BITS = 256;

class ChainUtils {
public:
    ChainUtils() = default;
    ~ChainUtils() noexcept = default;
    static bool isValidSHA256Hash(const std::string&) noexcept;
};

#endif