#include "service-handler.hpp"

//
//    void buildServiceProxy(IService& userService);
//    explicit ServiceHandler(nlohmann::json servicesConfigs);

ServiceHandler::ServiceHandler(nlohmann::json servicesConfigs)
    : m_servicesConfigs(servicesConfigs)

{
}

void ServiceHandler::buildServiceProxy(IService& userService)
{

    m_serviceMap.emplace(std::piecewise_construct,
        std::forward_as_tuple(userService.m_serviceId),
        std::forward_as_tuple(userService, m_servicesConfigs));
}

auto ServiceHandler::getProxyStatus(std::string serviceId) -> nlohmann::json
{
    nlohmann::json requiredStatus = m_serviceMap.at(serviceId).getStatus();
    return requiredStatus;
}

auto ServiceHandler::getAllProxyStatus() -> nlohmann::json
{
    std::map<std::string, ServiceProxy>::iterator itr;
    nlohmann::json allStatus;
    for(itr = m_serviceMap.begin(); itr != m_serviceMap.end(); itr++) {
        allStatus[itr->first] = itr->second.getStatus();
    }

    return allStatus;
}

