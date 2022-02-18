#include "daemon.hpp"

Daemon::Daemon(const std::string& filePath)
    : m_configHandler(filePath)
    , m_serviceHandler(m_configHandler.getAllConfig())
{
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
}
