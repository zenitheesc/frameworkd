#include "frameworkd/classes/service-handler/service-handler.hpp"

ServiceHandler::ServiceHandler(nlohmann::json servicesConfigs)
{
    for (auto& [serviceId, dependencies] : servicesConfigs["proxys"].items()) {
        std::map<std::string, ServiceProxy::ServiceState::state_t> depsMap;

        for (auto& [dependencyId, currState] : dependencies.items()) {
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
    try {
        return m_serviceMap.at(serviceId)->reportState();
    } catch (const std::out_of_range) {
        std::string errorMessage = "Get dependency state request from a not built service! Unknown Service = " + serviceId;

        throw std::invalid_argument(errorMessage);
    }
}

auto ServiceHandler::getAllProxyState() -> nlohmann::json
{
    nlohmann::json allStates;

    for (auto& [serviceId, proxy] : m_serviceMap) {
        allStates[serviceId] = proxy->reportState();
    }

    return allStates;
}

void ServiceHandler::changeDependencyState(std::string serviceId, std::string dependencyId, ServiceProxy::ServiceState::state_t newState)
{
    if (dependencyId == "THIS") {
        throw std::invalid_argument("THIS is a specially reserved dependency and don't accept outside changes of its state");
    }

    try {
        m_serviceMap.at(serviceId)->m_proxyConfigs.changeDep(dependencyId, newState);
    } catch (const std::out_of_range) {
        std::string errorMessage = "Change dependency state request from a not built service! Unknown Service = " + serviceId;

        throw std::invalid_argument(errorMessage);
    }
}

void ServiceHandler::updateServiceProxy(std::string serviceId)
{
    try {
        m_serviceMap.at(serviceId)->autoUpdate();
    } catch (const std::out_of_range) {
        std::string errorMessage = "Update request from a not built service! Unknown Service = " + serviceId;

        throw std::invalid_argument(errorMessage);
    }
}

void ServiceHandler::updateAllServiceProxys()
{
    for (auto& [serviceId, proxy] : m_serviceMap) {
        proxy->autoUpdate();
    }
}

void ServiceHandler::run()
{
    for (auto& [serviceId, proxy] : m_serviceMap) {
        proxy->m_proxyConfigs.changeDep("THIS", ServiceProxy::ServiceState::RUNNING);
        proxy->configure();
        proxy->autoUpdate();
    }
}
