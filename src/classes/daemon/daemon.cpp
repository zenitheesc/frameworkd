#include "frameworkd/classes/daemon/daemon.hpp"

Daemon::Daemon(const std::string& name)
    : m_configHandler("/etc/frameworkd/" + name + ".json")
    , m_serviceHandler(m_configHandler.getAllConfig())
{
    DBusHandler::start(m_configHandler["serviceId"]);
}

void Daemon::deploy(StaticService& userService)
{
    m_serviceHandler.buildServiceProxy(userService);
}

void Daemon::deploy(RoutineService& userService)
{
    m_serviceHandler.buildServiceProxy(userService);
}

void Daemon::run()
{
    m_serviceHandler.run();
    DBusHandler::finish();
}

auto Daemon::getConfigHandler() -> ConfigHandler
{
    return m_configHandler;
}
