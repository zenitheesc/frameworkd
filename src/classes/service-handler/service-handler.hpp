#pragma once
#include "../service-proxy/service-proxy.hpp"
#include "../service/service.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <string>

class ServiceHandler {
//private:
public:
    std::map<std::string, ServiceProxy*> m_serviceMap;
    std::map<std::string, std::map<std::string, Status::stateT>> m_proxyDepsMap;

public:
    auto getProxyStatus(std::string serviceId) -> nlohmann::json;
    auto getAllProxyStatus() -> nlohmann::json;

    void buildServiceProxy(IService& userService, ServiceProxy::proxyT proxyType);
    explicit ServiceHandler(nlohmann::json servicesConfigs);
    ~ServiceHandler();
};

