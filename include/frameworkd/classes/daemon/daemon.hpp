#pragma once

#include "../../utilities/config-handler/config-handler.hpp"
#include "../dbus-handler/dbus-handler.hpp"
#include "../service-handler/service-handler.hpp"

class Daemon {
public:
    explicit Daemon(const std::string& filePath);

    void deploy(StaticService& userService);
    void deploy(RoutineService& userService);

    void run();

    void getDaemonStatus();

    auto getConfigHandler() -> ConfigHandler;

private:
    ConfigHandler m_configHandler;
    ServiceHandler m_serviceHandler;
    DBusHandler m_dbusHandler;
};
