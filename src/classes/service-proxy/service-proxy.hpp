#pragma once
#include "../iservice/iservice.hpp"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

class ServiceProxy { 
protected:
    friend class ServiceHandler;
    friend class Tester;

    enum proxy_t { STATIC_SERVICE = 0,
        ROUTINE_SERVICE };

    class ServiceState {
    public:
        enum state_t { MISSING_DEPENDENCIES = 0,
            UNINITIALIZED,
            RUNNING,
            STOPPED,
            FINISHED,
            STAND_BY,
            UNKNOWN };

        const state_t m_state;

        explicit ServiceState(state_t state)
            : m_state(state)
        {
        }

        virtual void somethingIsMissing() { }
        virtual void allFine() { }
        [[nodiscard]] auto getState() const -> state_t;

    };

    class ProxyConfigs {
    public:
        struct Dependency {
            ServiceState::state_t m_reqrState;
            ServiceState::state_t m_currState;
            
            Dependency(ServiceState::state_t reqrState, ServiceState::state_t currState)
                : m_reqrState(reqrState)
                , m_currState(currState)
            {
            }

            Dependency() = default;
        };

        std::map<std::string, Dependency> m_depsMap;

        void changeDep(std::string dependencieId, ServiceState::state_t currState);
        explicit ProxyConfigs(std::map<std::string, ServiceState::state_t> depsMap);
    };

    proxy_t m_proxyType;
    std::mutex m_stateMtx;
    IService& m_realService;
    ProxyConfigs m_proxyConfigs;
    std::string m_realServiceId;
    std::unique_ptr<ServiceState> m_state;

    auto checkState() -> ServiceState::state_t;

    virtual void autoUpdate();
    virtual void serviceCycle() { }
    virtual void changeState(ServiceState::state_t newState) { }

public:
    ServiceProxy(IService& realService, proxy_t proxyType, std::map<std::string, ServiceState::state_t> depsMap)
        : m_realService(realService)
        , m_proxyType(proxyType)
        , m_proxyConfigs(depsMap)
        , m_realServiceId(realService.m_serviceId)
    {
    }

    virtual auto reportState() -> nlohmann::json;

    virtual ~ServiceProxy() = default;
};

