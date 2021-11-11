#include "service-handler.hpp"

ServiceHandler::ServiceHandler(nlohmann::json servicesConfigs)
{
    for(auto& [serviceId, dependencies] : servicesConfigs["proxys"].items()) {
        std::map<std::string, ServiceProxy::ServiceState::state_t> depsMap;

        for(auto& [dependencyId, currState] : dependencies.items()) {
            depsMap.emplace(std::pair<std::string, ServiceProxy::ServiceState::state_t>(dependencyId, currState));
        }
        m_proxyDepsMap.emplace(std::pair<std::string, std::map<std::string, ServiceProxy::ServiceState::state_t>>(serviceId, depsMap));
    }
}

void ServiceHandler::buildServiceProxy(StaticService& userService)
{
        m_serviceMap.emplace(std::make_pair(
                std::string(userService.m_serviceId),
                std::make_unique<StaticServiceProxy>(userService, m_proxyDepsMap.at(userService.m_serviceId))));
}

void ServiceHandler::buildServiceProxy(RoutineService& userService)
{
        m_serviceMap.emplace(std::make_pair(
                std::string(userService.m_serviceId),
                std::make_unique<RoutineServiceProxy>(userService, m_proxyDepsMap.at(userService.m_serviceId))));
}

auto ServiceHandler::getProxyState(std::string serviceId) -> nlohmann::json
{
    nlohmann::json requiredState = m_serviceMap.at(serviceId)->reportState();
    return requiredState;
}

auto ServiceHandler::getAllProxyState() -> nlohmann::json
{
    nlohmann::json allStates;

    for (auto& [serviceId, proxy] : m_serviceMap) {
        allStates[serviceId] = proxy->reportState();
    }

    return allStates;
}

void ServiceHandler::run() {
    
    for (auto& [serviceId , proxy] : m_serviceMap) {
        proxy->m_proxyConfigs.changeDep("THIS", ServiceProxy::ServiceState::RUNNING);
        proxy->autoUpdate();
    }
}
