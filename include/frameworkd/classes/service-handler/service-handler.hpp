#pragma once
#include "../iservice/iservice.hpp"
#include "../routine-service-proxy/routine-service-proxy.hpp"
#include "../service-proxy/service-proxy.hpp"
#include "../static-service-proxy/static-service-proxy.hpp"
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

class ServiceHandler {
public:
    explicit ServiceHandler(nlohmann::json servicesConfigs);

protected:
    std::map<std::string, std::unique_ptr<ServiceProxy>> m_serviceMap;
    std::map<std::string, std::map<std::string, ServiceProxy::ServiceState::state_t>> m_proxyDepsMap;

public:
    auto getProxyState(std::string serviceId) -> nlohmann::json;
    auto getAllProxyState() -> nlohmann::json;
    void buildServiceProxy(StaticService& userService);
    void buildServiceProxy(RoutineService& userService);
    void changeDependencyState(std::string serviceId, std::string dependencyId, ServiceProxy::ServiceState::state_t newState);
    void updateServiceProxy(std::string serviceId);
    void updateAllServiceProxys();
    void run();
};

