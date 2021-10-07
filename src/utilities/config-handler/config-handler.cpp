#include "config-handler.hpp"

ConfigHandler::ConfigHandler(std::string fileName)
    : m_fileName(fileName)
{
}

/**
 *@brief Validate the json
 */
void ConfigHandler::validateConfig()
{
    for (auto it : m_requiredFields) {
        if (!m_config.contains(it)) {
            throw std::runtime_error("Config file dosen't have all the required fields");
        }
    }
}

/**
 *@brief Reads a file and turn them into a json object
 */
void ConfigHandler::read()
{
    std::ifstream file(m_fileName);
    if (!file.is_open()) {
        throw std::invalid_argument("File not found");
    }

    file >> m_config;

    validateConfig();

    file.close();
}

/**
 *@brief Reads a file and turn them into a json object
 *@param fileName Name of the file to read
 */
void ConfigHandler::read(std::string fileName)
{
    m_fileName = fileName;
    read();
}

/**
 *@brief Gets a value from the json object
 *@param field Field to get the value 
 */
auto ConfigHandler::getConfig(std::string field) const -> const nlohmann::json
{
    return m_config[field];
}

/**
 *@brief Operator overload that encapsulates the getConfig()
 */
auto ConfigHandler::operator[](std::string field) const -> const nlohmann::json
{
    return getConfig(field);
}
