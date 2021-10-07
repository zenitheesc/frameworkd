#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

class ConfigHandler {
private:
    static nlohmann::json m_config;
    std::string m_fileName;
    std::vector<std::string> m_requiredFields {
        "data", "services", "serviceId"
    };

public:
    explicit ConfigHandler(std::string fileName);
    void read(std::string fileName);
    void read();
    auto operator[](std::string field) const -> const nlohmann::json;

private:
    [[nodiscard]] auto getConfig(std::string field) const -> const nlohmann::json;
    void validateConfig();
};
