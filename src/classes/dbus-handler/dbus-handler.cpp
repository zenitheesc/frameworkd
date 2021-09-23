#include "./dbus-handler.hpp"


DBusHandler::DBusHandler(std::string serviceName) : _isServer{true}, _serviceName{serviceName} {

    _connection = sdbus::createSystemBusConnection(serviceName);

}

DBusHandler::DBusHandler(std::string serviceName, bool isServer) : _isServer{isServer}, _serviceName{serviceName} {

    if (isServer) _connection = sdbus::createSystemBusConnection(serviceName);

}

sdbus::IObject* DBusHandler::findObject(PathHandler::DBusPath path) {

    try {

        return _DBusObjects.at(path.objectPath).get();

    } catch (std::out_of_range& e) {

        _DBusObjects[path.objectPath] = sdbus::createObject(*_connection, path.objectPath);
        return _DBusObjects[path.objectPath].get();

    }

}

sdbus::IProxy* DBusHandler::findProxy(PathHandler::DBusPath path) {

    std::string uniqueId = path.service + path.objectPath;

    try {

        return _DBusProxys.at(uniqueId).get();

    } catch (std::out_of_range& e) {

        _DBusProxys[uniqueId] = sdbus::createProxy(path.service, path.objectPath);
        return _DBusProxys[uniqueId].get();

    }

}

void DBusHandler::registerMethod(PathHandler::DBusPath path, DBusCallback callback) {

    sdbus::IObject* object = findObject(path);

    auto wrapper = [&callback](sdbus::MethodCall call) {

        std::vector<u_int8_t> bson;
        call >> bson;

        nlohmann::json answer = callback(nlohmann::json::from_bson(bson));

        auto reply = call.createReply();

        reply << nlohmann::json::to_bson(answer);

        reply.send();

    };

    object->registerMethod(path.interface, path.functionality, "ay", "ay", wrapper);

}

void DBusHandler::subscribeToSignal(PathHandler::DBusPath path, DBusVoidCallback callback) {

    sdbus::IProxy* proxy = findProxy(path);

    auto wrapper = [&callback](sdbus::Signal& signal) {

        std::vector<u_int8_t> bson;
        signal >> bson;

        callback(nlohmann::json::from_bson(bson));

    };

    proxy->registerSignalHandler(path.interface, path.functionality, wrapper);

}

void DBusHandler::registerSignal(PathHandler::DBusPath path) {
    
    sdbus::IObject* object = findObject(path);
    object->registerSignal(path.interface, path.functionality, "ay");

}

nlohmann::json DBusHandler::callMethod(PathHandler::DBusPath path, nlohmann::json arg) {

    sdbus::IProxy* proxy = findProxy(path);
    auto method = proxy->createMethodCall(path.interface, path.functionality);

    method << nlohmann::json::to_bson(arg);

    auto reply = proxy->callMethod(method);

    std::vector<u_int8_t> bson;
    reply >> bson;

    return nlohmann::json::from_bson(bson);

}

void DBusHandler::callMethodAsync(PathHandler::DBusPath path, nlohmann::json arg, DBusVoidCallback callback) {

    sdbus::IProxy* proxy = findProxy(path);
    auto method = proxy->createMethodCall(path.interface, path.functionality);

    method << nlohmann::json::to_bson(arg);

    auto wrapper = [&callback](sdbus::MethodReply& reply, const sdbus::Error* error) {

        std::vector<u_int8_t> bson;
        reply >> bson;

        callback(nlohmann::json::from_bson(bson));

    };

    auto reply = proxy->callMethod(method, wrapper);

}

void DBusHandler::emitSignal(PathHandler::DBusPath path, nlohmann::json arg) {

    if (!_started) return; //add error
    
    sdbus::IObject* object = findObject(path);

    auto signal = object->createSignal(path.interface, path.functionality);
    signal << nlohmann::json::to_bson(arg);

    object->emitSignal(signal);

}

void DBusHandler::finish() {

    for (auto const& proxy : _DBusProxys) {
        proxy.second->finishRegistration();
    }

    for (auto const& object : _DBusObjects) {
        object.second->finishRegistration();
    }

    _started = true;

    if (_isServer) _connection->enterEventLoop();

}

