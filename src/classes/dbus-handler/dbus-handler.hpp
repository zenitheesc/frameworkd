#include <sdbus-c++/sdbus-c++.h>

#include <map>
#include <nlohmann/json.hpp>
#include <string>

class DBusHandler {
   private:
    std::string objectPath;
    std::string interfaceName;

    std::unique_ptr<sdbus::IConnection> connection;

    sdbus::IObject* SDbusEmitter;
    std::unique_ptr<sdbus::IObject> DBusObject;
    std::map<std::string, std::unique_ptr<sdbus::IProxy>> DBusProxys;
    void getProxys(nlohmann::json DBusObjectConfig);
   public:

    DBusHandler(std::string serviceName, nlohmann::json DBusObjectConfig);
    void emitSignal(std::string signalName, nlohmann::json arg);
    nlohmann::json callMethod(std::string endpointName, nlohmann::json arg);
    void registerSignals(std::vector<std::string> signalsNames);
};