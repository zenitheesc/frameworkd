#include "dbus-handler.hpp"

sdbus::IObject* DBusHandler::findObject(const DBusHandler::Path& path)
{
    try {
        return _DBusObjects.at(path.objectPath).get();
    } catch (std::out_of_range& e) {
        _DBusObjects[path.objectPath] = sdbus::createObject(*_connection, path.objectPath);
        return _DBusObjects[path.objectPath].get();
    }
}

sdbus::IProxy* DBusHandler::findProxy(const DBusHandler::Path& path)
{
    const std::string uniqueId = path.service + path.objectPath;

    try {
        return _DBusProxys.at(uniqueId).get();
    } catch (std::out_of_range& e) {
        _DBusProxys[uniqueId] = sdbus::createProxy(path.service, path.objectPath);
        return _DBusProxys[uniqueId].get();
    }
}

DBusHandler::DBusHandler(const std::string& serviceName)
    : _isServer { true }
    , _serviceName { serviceName }
{
    _connection = sdbus::createSystemBusConnection(serviceName);
}

DBusHandler::DBusHandler(const std::string& serviceName, bool isServer)
    : _isServer { isServer }
    , _serviceName { serviceName }
{
    if (isServer) {
        _connection = sdbus::createSystemBusConnection(serviceName);
    }

DBusHandler::DBusHandler()
    : m_isServer { false } {};

void DBusHandler::registerMethod(const DBusHandler::Path& path, DBusCallback&& callback)
{
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
    sdbus::IObject* object = findObject(path);
    object->registerSignal(path.interface, path.functionality, "ay");
}

nlohmann::json DBusHandler::callMethod(const DBusHandler::Path& path, nlohmann::json arg)
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
    if (!_started)
        return; // add error

    sdbus::IObject* object = findObject(path);

    auto signal = object->createSignal(path.interface, path.functionality);
    signal << nlohmann::json::to_bson(arg);

    object->emitSignal(signal);
}

void DBusHandler::exposeProperty(const DBusHandler::Path& path, std::function<nlohmann::json()>&& getter,
    DBusVoidCallback&& setter)
{
    sdbus::IObject* object = findObject(path);

    auto getterWrapper = [getter]() { return nlohmann::json::to_bson(getter()); };

    auto setterWrapper = [setter](const std::vector<u_int8_t>& arg) { setter(nlohmann::json::from_bson(arg)); };

    object->registerProperty(path.functionality)
        .onInterface(path.interface)
        .withGetter(getterWrapper)
        .withSetter(setterWrapper);
}

nlohmann::json DBusHandler::getProperty(const DBusHandler::Path& path)
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
    for (auto const& proxy : _DBusProxys)
        proxy.second->finishRegistration();

    for (auto const& object : _DBusObjects)
        object.second->finishRegistration();

    _started = true;

    if (_isServer)
        _connection->enterEventLoop();
}
