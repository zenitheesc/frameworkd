#pragma once

#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>

namespace PathHandler {

    struct DBusPath{
        std::string service = "";
        std::string objectPath = "";
        std::string interface = "";
        std::string functionality = "";
    };

    static DBusPath parsePath(std::string path);

    static std::vector<std::string> splitPath(std::string path);

    static bool regexMatch(std::string path);
};
