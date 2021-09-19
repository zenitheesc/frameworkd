#pragma once

#include "../../utils/path-handler/path-handler.hpp"
#include <sdbus-c++/sdbus-c++.h>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <vector>

class DBusHandler {

   public:

    using DBusObjectMap = std::map<std::string, std::unique_ptr<sdbus::IObject>>;
    using DBusProxyMap = std::map<std::string, std::unique_ptr<sdbus::IProxy>>;
    using DBusVoidCallback = std::function<void (nlohmann::json req)>;
    using DBusCallback = std::function<nlohmann::json (nlohmann::json req)>;

   private:

    std::string _serviceName;
    bool _started;

    std::unique_ptr<sdbus::IConnection> _connection;

    DBusObjectMap _DBusObjects;
    DBusProxyMap _DBusProxys;

   private:

    sdbus::IObject* addObject(std::string objectPath);

    sdbus::IProxy* addProxy(std::string service, std::string objectPath);
    
    void registerMethod(PathHandler::dbusPath path, DBusCallback callback);

    void subscribeToSignal(PathHandler::dbusPath path, DBusVoidCallback callback);

    void registerSignal(PathHandler::dbusPath path);
    
    nlohmann::json callMethod(PathHandler::dbusPath path, nlohmann::json arg);

    void callMethodAsync(PathHandler::dbusPath path, nlohmann::json arg, DBusVoidCallback callback);

    void emitSignal(PathHandler::dbusPath path, nlohmann::json arg);

    

   public:

    DBusHandler(std::string serviceName);

    DBusHandler(std::string serviceName, DBusObjectMap DBusObjects , DBusProxyMap DBusProxys);

    DBusHandler(std::string serviceName, DBusObjectMap DBusObjects , DBusProxyMap DBusProxys, std::unique_ptr<sdbus::IConnection> connection);

    nlohmann::json call(std::string dbusPath, nlohmann::json req);

    nlohmann::json call(std::string dbusPath, nlohmann::json req, DBusCallback callback);

    nlohmann::json expose(std::string dbusPath, nlohmann::json req, DBusCallback getter, DBusVoidCallback setter);

    nlohmann::json get(std::string dbusPath, nlohmann::json req);

    nlohmann::json get(std::string dbusPath, nlohmann::json req, DBusVoidCallback callback);

    nlohmann::json set(std::string dbusPath, nlohmann::json req);

    nlohmann::json subscribe(std::string dbusPath, DBusCallback callback);

    nlohmann::json subscribe(std::string dbusPath, DBusCallback callback, nlohmann::json dependencies);

    nlohmann::json bind(std::string dbusPath, DBusCallback callback);

    nlohmann::json bind(std::string dbusPath, DBusCallback callback, nlohmann::json dependencies);

    nlohmann::json emit(std::string dbusPath, nlohmann::json arg);

    void finish();
    
};