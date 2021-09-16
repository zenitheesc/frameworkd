#include "./dbus-handler.hpp"

#include <array>
#include <vector>

DBusHandler::DBusHandler(std::string serviceName, nlohmann::json DBusObjectConfig) {
    std::string objectNameTemp = DBusObjectConfig["id"];
    interfaceName = objectPath = serviceName + "." + objectNameTemp;

    std::replace(objectPath.begin(), objectPath.end(), ".", "/");
    objectPath = "/" + objectPath;

    DBusObject = sdbus::createObject(*connection, objectPath);
    SDbusEmitter = DBusObject.get();

    std::vector<std::string> signalsNames = DBusObjectConfig["emittedSignals"];
    registerSignals(signalsNames);
}

/**
 * 
 *@todo 
 * Adicionar regex para o endpoint name /word/word/word...
 * Adicionar callMethod async(?)
 */
nlohmann::json DBusHandler::callMethod(std::string endpointName, nlohmann::json arg) {
    if (DBusProxys.count(endpointName) != true) throw std::invalid_argument("endpoint not found");

    std::vector<std::uint8_t> response;
    std::string destinationInterface = endpointName;

    destinationInterface.erase(0, 1);
    std::replace(destinationInterface.begin(), destinationInterface.end(), "/", ".");

    auto method = DBusProxys[endpointName]->createMethodCall(interfaceName, endpointName);
    method << nlohmann::json::to_bson(arg);
    auto reply = DBusProxys[endpointName]->callMethod(method);

    reply >> response;

    return nlohmann::json::from_bson(response);
}

void DBusHandler::emitSignal(std::string signalName, nlohmann::json arg) {
    auto signal = SDbusEmitter->createSignal(interfaceName, signalName);
    signal << nlohmann::json::to_bson(arg);
    SDbusEmitter->emitSignal(signal);
}

void DBusHandler::registerSignals(std::vector<std::string> signalsNames) {
    for (int i = 0; i < signalsNames.size(); i++) {
        DBusObject->registerSignal(interfaceName, signalsNames[i], "ai");
    }
}

void DBusHandler::getProxys(nlohmann::json DBusObjectConfig) {
    std::vector<nlohmann::json> tempDependencies = DBusObjectConfig["dependencies"];

    for(const& dependency : tempDependencies){

    }
}

void bind() {
}
