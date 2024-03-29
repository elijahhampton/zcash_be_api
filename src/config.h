#ifndef CONFIG_H
#define CONFIG_H

#include <cstdlib>
#include <string>
#include <stdexcept>

class Config {
public:
    Config() = default;
    ~Config() noexcept = default;
    static std::string getEnv(const char* key, const std::string& defaultValue = "") {
        char* val = std::getenv(key);
        if (val == nullptr) {  // Use default value if not found
            if (defaultValue.empty()) {
                throw std::runtime_error(std::string("Environment variable ") + key + " is not set.");
            }
            return defaultValue;
        }
        return std::string(val);
    }

    static std::string getDatabaseHost() {
        return getEnv("DB_HOST", "localhost");
    }

    static std::string getDatabasePort() {
        return getEnv("DB_PORT", "5432");  // Default PostgreSQL port
    }

    static std::string getDatabaseName() {
        return getEnv("DB_NAME", "postgres");
    }

    static std::string getDatabaseUser() {
        return getEnv("DB_USER", "postgres");
    }

    static std::string getDatabasePassword() {
        return getEnv("DB_PASSWORD", "mysecretpassword");
    }

    static std::string getApiPort() {
        return getEnv("PORT", "8000");
    }

    static std::string getAccessControlOrigin() {
        return getEnv("ACCESS_CONTROL_ORIGIN", "*");
    }
};

#endif // CONFIG_H
