#include "service-proxy.hpp"
#include <iostream>

auto ServiceProxy::ServiceState::getState() const -> stateT
{
    return m_state;
}

ServiceProxy::ProxyConfigs::ProxyConfigs(std::map<std::string, ServiceState::stateT> depsMap)
{
    for (auto& [depId, depReqState] : depsMap) {
        m_depsMap.emplace(std::piecewise_construct,
            std::forward_as_tuple(depId),
            std::forward_as_tuple(depReqState, ServiceState::UNKNOWN));
    }

    m_depsMap.emplace(std::piecewise_construct,
        std::forward_as_tuple("THIS"),
        std::forward_as_tuple(ServiceState::RUNNING, ServiceState::UNKNOWN));
}

void ServiceProxy::ProxyConfigs::changeDep(std::string dependencieId, ServiceState::stateT currState)
{
    m_depsMap[dependencieId].m_currState = currState;
}

