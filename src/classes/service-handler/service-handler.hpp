#pragma once
#include "../iservice/iservice.hpp"
#include "../routine-service-proxy/routine-service-proxy.hpp"
#include "../service-proxy/service-proxy.hpp"
#include "../static-service-proxy/static-service-proxy.hpp"
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

class ServiceHandler {
protected:
    friend class Tester;

    std::map<std::string, std::unique_ptr<ServiceProxy>> m_serviceMap;
    std::map<std::string, std::map<std::string, ServiceProxy::ServiceState::stateT>> m_proxyDepsMap;

public:
    auto getProxyState(std::string serviceId) -> nlohmann::json;
    auto getAllProxyState() -> nlohmann::json;
    void buildServiceProxy(StaticService& userService);
    void buildServiceProxy(RoutineService& userService);
    void run();

    explicit ServiceHandler(nlohmann::json servicesConfigs);
};

