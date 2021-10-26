#include "service-handler.hpp"

ServiceHandler::ServiceHandler(nlohmann::json servicesConfigs)
{
    //JSON DE CONFIGS
    for (nlohmann::json::iterator it = servicesConfigs["proxys"].begin(); it != servicesConfigs["proxys"].end(); it++) {
        std::map<std::string, Status::stateT> depsMap;

        for (nlohmann::json::iterator itr = it.value().begin(); itr != it.value().end(); itr++) {
            depsMap.insert(std::pair<std::string, Status::stateT>(itr.key(), itr.value()));
        }
        m_proxyDepsMap.insert(std::pair<std::string, std::map<std::string, Status::stateT>>(it.key(), depsMap));
    }
}

void ServiceHandler::buildServiceProxy(IService& userService, ServiceProxy::proxyT proxyType)
{
    if (proxyType == ServiceProxy::STATIC_SERVICE) {
        m_serviceMap.insert(std::pair<std::string, ServiceProxy*>(userService.m_serviceId, new staticService::Proxy(userService, m_proxyDepsMap.at(userService.m_serviceId))));

    } else {
        m_serviceMap.insert(std::pair<std::string, ServiceProxy*>(userService.m_serviceId, new routineService::Proxy(userService, m_proxyDepsMap.at(userService.m_serviceId))));
    }
}

auto ServiceHandler::getProxyStatus(std::string serviceId) -> nlohmann::json
{
    nlohmann::json requiredStatus = m_serviceMap.at(serviceId)->reportStatus();
    return requiredStatus;
}

auto ServiceHandler::getAllProxyStatus() -> nlohmann::json
{
    std::map<std::string, ServiceProxy*>::iterator itr;
    nlohmann::json allStatus;
    for (itr = m_serviceMap.begin(); itr != m_serviceMap.end(); itr++) {
        allStatus[itr->first] = itr->second->reportStatus();
    }

    return allStatus;
}

ServiceHandler::~ServiceHandler() {
    std::map<std::string, ServiceProxy*>::iterator itr;
    for (itr = m_serviceMap.begin(); itr != m_serviceMap.end(); itr++) {
        delete itr->second; 
    }
}
