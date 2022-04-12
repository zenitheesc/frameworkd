#include "frameworkd/utilities/config-handler/config-handler.hpp"
#include <iostream>

nlohmann::json ConfigHandler::m_config;

ConfigHandler::ConfigHandler(const std::string& fileName)
    : m_fileName(fileName)
{
    read();
}

/**
 * @brief Validate the json
 */
void ConfigHandler::validateConfig()
{
    for (const auto& it : m_requiredFields) {
        if (!ConfigHandler::m_config.contains(it)) {
            throw std::runtime_error("Config file doesn't have all the required fields");
        }
    }
}

/**
 * @brief Reads a file and turn them into a json object
 */
void ConfigHandler::read()
{
    std::ifstream file(m_fileName);
    if (!file.is_open()) {
        throw std::invalid_argument("Configuration file not found");
    }

    file >> ConfigHandler::m_config;

    validateConfig();

    file.close();
}

/**
 * @brief Reads a file and turn them into a json object
 * @param fileName Name of the file to read
 */
void ConfigHandler::read(const std::string& fileName)
{
    m_fileName = fileName;
    read();
}

/**
 * @brief Gets a value from the json object
 * @param field Field to get the value
 */
auto ConfigHandler::getConfig(const std::string& field) const -> const nlohmann::json
{
    return ConfigHandler::m_config[field];
}

/**
 * @brief Operator overload that encapsulates the getConfig()
 */
auto ConfigHandler::operator[](const std::string& field) const -> const nlohmann::json
{
    return getConfig(field);
}

auto ConfigHandler::getAllConfig() const -> const nlohmann::json
{
    return m_config;
}
