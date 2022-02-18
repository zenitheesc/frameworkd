#include <fstream>
#include <nlohmann/json.hpp>

class ConfigHandler {
private:
    static nlohmann::json m_config;
    std::string m_fileName;
    const std::vector<std::string> m_requiredFields {
        "data", "services", "serviceId"
    };

public:
    explicit ConfigHandler(const std::string& fileName);
    void read(const std::string& fileName);
    void read();
    auto operator[](const std::string& field) const -> const nlohmann::json;
    auto getAllConfig() const -> const nlohmann::json;

private:
    [[nodiscard]] auto getConfig(const std::string& field) const -> const nlohmann::json;
    void validateConfig();
};
