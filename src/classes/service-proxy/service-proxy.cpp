#include "service-proxy.hpp"

ServiceProxy::ProxyConfigs::ProxyConfigs(std::map<std::string, ServiceState::state_t> depsMap)
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

auto ServiceProxy::ServiceState::getState() const -> state_t
{
    return m_state;
}

void ServiceProxy::ServiceProxy::autoUpdate()
{
    bool noMissingDependencies = true;

    for (auto& [depId, dependency] : m_proxyConfigs.m_depsMap) {
        if (dependency.m_currState != dependency.m_reqrState) {
            noMissingDependencies = false;
        }
    }

    if (noMissingDependencies) {
        m_state->allFine();
    } else {
        m_state->somethingIsMissing();
    }
}

auto ServiceProxy::ServiceProxy::checkState() -> ServiceState::state_t
{
    const std::lock_guard<std::mutex> lock { m_stateMtx };
    return m_state->getState();
}

auto ServiceProxy::ServiceProxy::reportState() -> nlohmann::json
{
    return (nlohmann::json) { { "serviceId", m_realServiceId }, { "State", checkState() } };
}

void ServiceProxy::ProxyConfigs::changeDep(std::string dependencieId, ServiceState::state_t currState)
{
    m_depsMap.at(dependencieId).m_currState = currState;
}

void ServiceProxy::configure()
{
    m_realService.setup();
}
