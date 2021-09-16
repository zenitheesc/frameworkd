#include "./dbus-controller.hpp"

DBusController::DBusController(std::string serviceName) : serviceName{serviceName} {
    connection = sdbus::createSystemBusConnection(serviceName);
}