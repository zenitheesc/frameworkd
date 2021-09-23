#include "./dbus-name-handler.hpp"

DBusName::DBusName(std::string serviceId) {
    serviceIdDots = "org.frameworkd." + serviceId;
    serviceIdSlashes = "/" + serviceIdDots;
    std::replace(serviceIdSlashes.begin(), serviceIdSlashes.end(), '.', '/');
}

std::string DBusName::getDaemonId() {
    return serviceIdDots;
}

std::string DBusName::getObjectPath(std::string objectName) {
    return serviceIdSlashes + "/" + objectName;
}

std::string DBusName::getInterfaceName(std::string interfaceName) {
    return serviceIdDots + "." + interfaceName;
}

nlohmann::json DBusName::parseFuncionalityPath(std::string functionalityPath) {
    std::replace(functionalityPath.begin(), functionalityPath.end(), '/', ' ');
    std::vector<std::string> array;
    std::stringstream ss(functionalityPath);

    std::string word;
    while (ss >> word) array.push_back(word);

    nlohmann::json parsedResponse;

    parsedResponse["serviceId"] = array[0] + "." + array[1] + "." + array[2];
    parsedResponse["objectPath"] = "/" + array[0] + "/" + array[1] + "/" + array[2] + "/" + array[3];
    parsedResponse["interfaceName"] = array[0] + "." + array[1] + "." + array[2] +"." + array[3];
    parsedResponse["functionalityName"] = array[4];

    return parsedResponse;
}
