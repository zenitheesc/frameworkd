#pragma once
#include "../service-proxy/service-proxy.hpp"
#include "../service/service.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <string>

class ServiceHandler {
//private:
public:
    std::map<std::string, ServiceProxy> m_serviceMap;
    nlohmann::json m_servicesConfigs;

public:
    auto getProxyStatus(std::string serviceId) -> nlohmann::json;
    auto getAllProxyStatus() -> nlohmann::json;

    void buildServiceProxy(IService& userService);
    explicit ServiceHandler(nlohmann::json servicesConfigs);
};

