#include <sdbus-c++/sdbus-c++.h>

#include <map>
#include <string>
class DBusController {
   private:
    std::string serviceName;

    std::unique_ptr<sdbus::IConnection> connection;
    std::map<std::string, std::unique_ptr<sdbus::IObject>> DBusObjects;

   public:
    DBusController(std::string serviceName);

    void bindSignal(std::string signalName, void (*signalHandler)(std::string json));
    void bindEndpoint(std::string endpointName, sdbus::method_callback signalHandler);
    void startListen();

    ~DBusController();
};
