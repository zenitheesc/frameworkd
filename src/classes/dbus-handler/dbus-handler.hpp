#pragma once

#include <exception>
#include <map>
#include <nlohmann/json.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <sstream>
#include <string>
#include <vector>

class DBusHandler {

public:
    using DBusObjectMap = std::map<std::string, std::unique_ptr<sdbus::IObject>>;
    using DBusProxyMap = std::map<std::string, std::unique_ptr<sdbus::IProxy>>;
    using DBusVoidCallback = std::function<void(nlohmann::json req)>;
    using DBusCallback = std::function<nlohmann::json(nlohmann::json req)>;

    struct Path {

        std::string service;
        std::string objectPath;
        std::string interface;
        std::string functionality;

        Path() = default;

        Path(const std::string& service, const std::string& objectPath, const std::string& interface,
            const std::string& functionality)
            : service { std::move(service) }
            , objectPath { std::move(objectPath) }
            , interface { std::move(interface) }
            , functionality { std::move(functionality) }
        {
            if (service.empty() || objectPath.empty() || interface.empty() || functionality.empty()) {
                throw std::invalid_argument("Invalid path: empty string doesn't satisfies path format");
            }
        };
    };

private:
    std::string m_serviceName;
    bool m_started;
    bool m_isServer;

    std::unique_ptr<sdbus::IConnection> m_connection;

    DBusObjectMap m_DBusObjects;
    DBusProxyMap m_DBusProxys;

    auto findProxy(const DBusHandler::Path& path) -> sdbus::IProxy*;

    auto findObject(const DBusHandler::Path& path) -> sdbus::IObject*;

public:
    DBusHandler();

    explicit DBusHandler(const std::string& serviceName);

    DBusHandler(const std::string& serviceName, DBusObjectMap DBusObjects, DBusProxyMap DBusProxys);

    DBusHandler(const std::string& serviceName, DBusObjectMap DBusObjects, DBusProxyMap DBusProxys,
        std::unique_ptr<sdbus::IConnection> connection);

    void registerMethod(const DBusHandler::Path& path, DBusCallback&& callback);

    void subscribeToSignal(const DBusHandler::Path& path, DBusVoidCallback&& callback);

    void registerSignal(const DBusHandler::Path& path);

    auto callMethod(const DBusHandler::Path& path, nlohmann::json arg) -> nlohmann::json;

    void callMethodAsync(const DBusHandler::Path& path, nlohmann::json arg, DBusVoidCallback&& callback);

    void emitSignal(const DBusHandler::Path& path, nlohmann::json arg);

    void exposeProperty(const DBusHandler::Path& path, std::function<nlohmann::json()>&& getter,
        DBusVoidCallback&& setter);

    auto getProperty(const DBusHandler::Path& path) -> nlohmann::json;

    void getProperty(const DBusHandler::Path& path, DBusVoidCallback&& callback);

    void setProperty(const DBusHandler::Path& path, nlohmann::json arg);

    void finish();
};
