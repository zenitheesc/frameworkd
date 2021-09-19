#include "./dbus-handler.hpp"



DBusHandler::DBusHandler(std::string serviceName) {
    _serviceName = "org.frameworkd." + serviceName;
    _connection = sdbus::createSystemBusConnection(_serviceName);
}

sdbus::IObject* DBusHandler::addObject(std::string objectPath) {
    _DBusObjects[objectPath] = sdbus::createObject(*_connection, objectPath);
    return _DBusObjects[objectPath].get();
}

sdbus::IProxy* DBusHandler::addProxy(std::string destinationService, std::string objectPath) {

    std::string uniqueId = destinationService + objectPath;
    
    _DBusProxys[uniqueId] = sdbus::createProxy(destinationService, objectPath);

    return _DBusProxys[uniqueId].get();
}

void DBusHandler::registerMethod(PathHandler::dbusPath path, DBusCallback callback) {

    auto exists = _DBusObjects.find(path.objectPath);
    auto object = (exists != _DBusObjects.end())? exists->second.get() : addObject(path.objectPath);

    auto wrapper = [&callback](sdbus::MethodCall call) {

        std::vector<u_int8_t> bson;
        call >> bson;

        nlohmann::json answer = callback(nlohmann::json::from_bson(bson));

        auto reply = call.createReply();

        reply << nlohmann::json::to_bson(answer);

        reply.send();

    };

    object->registerMethod(path.interface, path.functionality, "ai", "ai", wrapper);

}

void DBusHandler::subscribeToSignal(PathHandler::dbusPath path, DBusVoidCallback callback) {

    std::string uniqueId = path.service + path.objectPath;

    auto exists = _DBusProxys.find(uniqueId);
    auto proxy = (exists != _DBusProxys.end())? exists->second.get() : addProxy(path.service, path.objectPath);

    auto wrapper = [&callback](sdbus::Signal& signal) {

        std::vector<u_int8_t> bson;
        signal >> bson;

        callback(nlohmann::json::from_bson(bson));

    };

    proxy->registerSignalHandler(path.interface, path.functionality, wrapper);

}

void DBusHandler::registerSignal(PathHandler::dbusPath path) {
    
    auto exists = _DBusObjects.find(path.objectPath);
    auto object = (exists != _DBusObjects.end())? exists->second.get() : addObject(path.objectPath);

    object->registerSignal(path.interface, path.functionality, "ai");

}

nlohmann::json DBusHandler::callMethod(PathHandler::dbusPath path, nlohmann::json arg) {
    if(!_started) return; //add error

    std::string uniqueId = path.service + path.objectPath;

    auto exists = _DBusProxys.find(uniqueId);
    auto proxy = (exists != _DBusProxys.end())? exists->second.get() : addProxy(path.service, path.objectPath);

    auto method = proxy->createMethodCall(path.interface, path.functionality);

    method << nlohmann::json::to_bson(arg);

    auto reply = proxy->callMethod(method);

    std::vector<u_int8_t> bson;

    reply >> bson;

    return nlohmann::json::from_bson(bson);

}

void DBusHandler::callMethodAsync(PathHandler::dbusPath path, nlohmann::json arg, DBusVoidCallback callback) {
    if(!_started) return; //add error

    std::string uniqueId = path.service + path.objectPath;

    auto exists = _DBusProxys.find(uniqueId);
    auto proxy = (exists != _DBusProxys.end())? exists->second.get() : addProxy(path.service, path.objectPath);

    auto method = proxy->createMethodCall(path.interface, path.functionality);

    method << nlohmann::json::to_bson(arg);

    auto wrapper = [&callback](sdbus::MethodReply& reply, const sdbus::Error* error) {

        std::vector<u_int8_t> bson;
        reply >> bson;

        callback(nlohmann::json::from_bson(bson));

    };

    auto reply = proxy->callMethod(method, wrapper);
}

void DBusHandler::emitSignal(PathHandler::dbusPath path, nlohmann::json arg) {
    if(!_started) return; //add error
    
    auto exists = _DBusObjects.find(path.objectPath);
    auto object = (exists != _DBusObjects.end())? exists->second.get() : addObject(path.objectPath);

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

    _connection->enterEventLoop();
}
