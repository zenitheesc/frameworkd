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
    bool _isServer;

    std::unique_ptr<sdbus::IConnection> _connection;

    DBusObjectMap _DBusObjects;
    DBusProxyMap _DBusProxys;

   public:

    sdbus::IObject* addObject(std::string objectPath);

    sdbus::IProxy* addProxy(std::string service, std::string objectPath);
    
    void registerMethod(PathHandler::DBusPath path, DBusCallback callback);

    void subscribeToSignal(PathHandler::DBusPath path, DBusVoidCallback callback);

    void registerSignal(PathHandler::DBusPath path);
    
    nlohmann::json callMethod(PathHandler::DBusPath path, nlohmann::json arg);

    void callMethodAsync(PathHandler::DBusPath path, nlohmann::json arg, DBusVoidCallback callback);

    void emitSignal(PathHandler::DBusPath path, nlohmann::json arg);

   public:

    DBusHandler(std::string serviceName);

    DBusHandler(std::string serviceName, bool isServer);

    DBusHandler(std::string serviceName, DBusObjectMap DBusObjects , DBusProxyMap DBusProxys);

    DBusHandler(std::string serviceName, DBusObjectMap DBusObjects , DBusProxyMap DBusProxys, std::unique_ptr<sdbus::IConnection> connection);

    nlohmann::json call(std::string DBusPath, nlohmann::json req);

    nlohmann::json call(std::string DBusPath, nlohmann::json req, DBusCallback callback);

      void expose(PathHandler::DBusPath path, DBusCallback getter, DBusVoidCallback setter);

      nlohmann::json get(PathHandler::DBusPath path);

      void get(PathHandler::DBusPath path , DBusVoidCallback callback);

      void set(PathHandler::DBusPath path, nlohmann::json arg);

    nlohmann::json subscribe(std::string DBusPath, DBusCallback callback);

    nlohmann::json subscribe(std::string DBusPath, DBusCallback callback, nlohmann::json dependencies);

    nlohmann::json bind(std::string DBusPath, DBusCallback callback);

    nlohmann::json bind(std::string DBusPath, DBusCallback callback, nlohmann::json dependencies);

    nlohmann::json emit(std::string DBusPath, nlohmann::json arg);

    void finish();
    
};