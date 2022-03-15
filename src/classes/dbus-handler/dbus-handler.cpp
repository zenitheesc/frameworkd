#include "dbus-handler.hpp"

std::string DBusHandler::s_serviceName = "";
bool DBusHandler::s_started = false;
bool DBusHandler::s_isServer = false;

std::unique_ptr<sdbus::IConnection> DBusHandler::s_connection;

DBusHandler::DBusObjectMap  DBusHandler::s_DBusObjects;
DBusHandler::DBusProxyMap DBusHandler::s_DBusProxys;

void DBusHandler::start(const std::string& serviceName) {
    s_isServer = true;
    s_serviceName = "zfkd.dbus." + serviceName;
    s_connection = sdbus::createSystemBusConnection(s_serviceName);
}

void DBusHandler::start() {
    s_isServer = false;
};

auto DBusHandler::findObject(const DBusHandler::Path& path) -> sdbus::IObject*
{
    try {
        return s_DBusObjects.at(path.objectPath).get();
    } catch (std::out_of_range& e) {
        s_DBusObjects[path.objectPath] = sdbus::createObject(*s_connection, path.objectPath);
        return s_DBusObjects[path.objectPath].get();
    }
}

auto DBusHandler::findProxy(const DBusHandler::Path& path) -> sdbus::IProxy*
{
    const std::string uniqueId = path.service + path.objectPath;

    try {
        return s_DBusProxys.at(uniqueId).get();
    } catch (std::out_of_range& e) {
        s_DBusProxys[uniqueId] = sdbus::createProxy(path.service, path.objectPath);
        return s_DBusProxys[uniqueId].get();
    }
}

void DBusHandler::registerMethod(const DBusHandler::Path& path, DBusCallback&& callback)
{
    if (!s_isServer) {
        throw std::logic_error("Only servers can register methods");
    }

    if (s_started) {
        throw std::logic_error("Methods should be register before finishing the handler");
    }

    sdbus::IObject* object = findObject(path);

    auto wrapper = [callback](sdbus::MethodCall call) {
        std::vector<u_int8_t> bson;
        call >> bson;

        nlohmann::json answer = callback(nlohmann::json::from_bson(bson));

        auto reply = call.createReply();

        reply << nlohmann::json::to_bson(answer);

        reply.send();
    };

    object->registerMethod(path.interface, path.functionality, "ay", "ay", wrapper);
}

void DBusHandler::subscribeToSignal(const DBusHandler::Path& path, DBusVoidCallback&& callback)
{
    sdbus::IProxy* proxy = findProxy(path);

    auto wrapper = [callback](sdbus::Signal& signal) {
        std::vector<u_int8_t> bson;
        signal >> bson;

        callback(nlohmann::json::from_bson(bson));
    };

    proxy->registerSignalHandler(path.interface, path.functionality, wrapper);
}

void DBusHandler::registerSignal(const DBusHandler::Path& path)
{
    if (!s_isServer) {
        throw std::logic_error("Only servers can register signals");
    }

    if (s_started) {
        throw std::logic_error("register signals is only possible before finishing the handler");
    }

    sdbus::IObject* object = findObject(path);
    object->registerSignal(path.interface, path.functionality, "ay");
}

auto DBusHandler::callMethod(const DBusHandler::Path& path, nlohmann::json arg) -> nlohmann::json
{
    sdbus::IProxy* proxy = findProxy(path);
    auto method = proxy->createMethodCall(path.interface, path.functionality);

    method << nlohmann::json::to_bson(arg);

    auto reply = proxy->callMethod(method);

    std::vector<u_int8_t> bson;
    reply >> bson;
    return nlohmann::json::from_bson(bson);
}

void DBusHandler::callMethodAsync(const DBusHandler::Path& path, nlohmann::json arg, DBusVoidCallback&& callback)
{
    sdbus::IProxy* proxy = findProxy(path);
    auto method = proxy->createMethodCall(path.interface, path.functionality);

    method << nlohmann::json::to_bson(arg);

    auto wrapper = [callback](sdbus::MethodReply& reply, const sdbus::Error* error) {
        std::vector<u_int8_t> bson;
        reply >> bson;

        callback(nlohmann::json::from_bson(bson));
    };

    auto reply = proxy->callMethod(method, wrapper);
}

void DBusHandler::emitSignal(const DBusHandler::Path& path, nlohmann::json arg)
{
    if (!s_isServer) {
        throw std::logic_error("Only servers can emit signals");
    }

    if (!s_started) {
        throw std::logic_error("emit a signal is only possible after finishing the handler");
    }

    sdbus::IObject* object = findObject(path);

    auto signal = object->createSignal(path.interface, path.functionality);
    signal << nlohmann::json::to_bson(arg);

    object->emitSignal(signal);
}

void DBusHandler::exposeProperty(const DBusHandler::Path& path, std::function<nlohmann::json()>&& getter,
    DBusVoidCallback&& setter)
{

    if (!s_isServer) {
        throw std::logic_error("Only servers can expose properties");
    }

    if (s_started) {
        throw std::logic_error("expose a property is only possible before finishing the handler");
    }

    sdbus::IObject* object = findObject(path);

    auto getterWrapper = [getter]() { return nlohmann::json::to_bson(getter()); };

    auto setterWrapper = [setter](const std::vector<u_int8_t>& arg) { setter(nlohmann::json::from_bson(arg)); };

    object->registerProperty(path.functionality)
        .onInterface(path.interface)
        .withGetter(getterWrapper)
        .withSetter(setterWrapper);
}

auto DBusHandler::getProperty(const DBusHandler::Path& path) -> nlohmann::json
{
    sdbus::IProxy* proxy = findProxy(path);
    std::vector<u_int8_t> property = proxy->getProperty(path.functionality).onInterface(path.interface);

    return nlohmann::json::from_bson(property);
}

void DBusHandler::getProperty(const DBusHandler::Path& path, DBusVoidCallback&& callback)
{
    sdbus::IProxy* proxy = findProxy(path);
    std::vector<u_int8_t> property = proxy->getProperty(path.functionality).onInterface(path.interface);

    callback(nlohmann::json::from_bson(property));
}

void DBusHandler::setProperty(const DBusHandler::Path& path, nlohmann::json arg)
{
    sdbus::IProxy* proxy = findProxy(path);
    proxy->setProperty(path.functionality).onInterface(path.interface).toValue(nlohmann::json::to_bson(arg));
}

void DBusHandler::finish()
{

    s_started = true;

    for (auto const& object : s_DBusObjects) {
        object.second->finishRegistration();
    }

    for (auto const& proxy : s_DBusProxys) {
        proxy.second->finishRegistration();
    }

    if (s_isServer) {
        s_connection->enterEventLoop();
    }
}
