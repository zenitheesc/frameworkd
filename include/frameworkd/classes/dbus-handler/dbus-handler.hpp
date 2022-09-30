#pragma once

#include <exception>
#include <map>
#include <nlohmann/json.hpp>
#include <sdbus-c++/sdbus-c++.h>
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

        Path(const std::string& serviceName, const std::string& serviceObjectPath, const std::string& serviceInterface,
            const std::string& serviceFunctionality)
            : service { std::move(serviceName) }
            , objectPath { std::move(serviceObjectPath) }
            , interface { std::move(serviceInterface) }
            , functionality { std::move(serviceFunctionality) }
        {
            if (service.empty() || objectPath.empty() || interface.empty() || functionality.empty()) {
                throw std::invalid_argument("Invalid path: empty string doesn't satisfies path format");
            }
        };
    };

private:
    static std::string s_serviceName;
    static bool s_started;
    static bool s_isServer;

    static std::unique_ptr<sdbus::IConnection> s_connection;

    static DBusObjectMap s_DBusObjects;
    static DBusProxyMap s_DBusProxys;

    static auto findProxy(const DBusHandler::Path& path) -> sdbus::IProxy*;

    static auto findObject(const DBusHandler::Path& path) -> sdbus::IObject*;

public:
    static void start();

    static void start(const std::string& serviceName);

    static void start(const std::string& serviceName, DBusObjectMap DBusObjects, DBusProxyMap DBusProxys);

    static void start(const std::string& serviceName, DBusObjectMap DBusObjects, DBusProxyMap DBusProxys,
        std::unique_ptr<sdbus::IConnection> connection);

    static void registerMethod(const DBusHandler::Path& path, DBusCallback&& callback);

    static void subscribeToSignal(const DBusHandler::Path& path, DBusVoidCallback&& callback);

    static void registerSignal(const DBusHandler::Path& path);

    static auto callMethod(const DBusHandler::Path& path, nlohmann::json arg) -> nlohmann::json;

    static void callMethodAsync(const DBusHandler::Path& path, nlohmann::json arg, DBusVoidCallback&& callback);

    static void emitSignal(const DBusHandler::Path& path, nlohmann::json arg);

    static void exposeProperty(const DBusHandler::Path& path, std::function<nlohmann::json()>&& getter,
        DBusVoidCallback&& setter);

    static auto getProperty(const DBusHandler::Path& path) -> nlohmann::json;

    static void getProperty(const DBusHandler::Path& path, DBusVoidCallback&& callback);

    static void setProperty(const DBusHandler::Path& path, nlohmann::json arg);

    static void finish();
};
