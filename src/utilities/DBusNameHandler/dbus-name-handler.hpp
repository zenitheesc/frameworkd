#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>

class DBusName {
   private:

    std::string serviceIdDots;
    std::string serviceIdSlashes;

   public:

    DBusName(std::string serviceId);
    std::string getObjectPath(std::string objectName);
    std::string getDaemonId();
    std::string getInterfaceName(std::string interfaceName);

    static nlohmann::json parseFuncionalityPath(std::string funcionalityPath);

};