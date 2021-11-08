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

    enum proxyT { STATIC_SERVICE = 0,
        ROUTINE_SERVICE };

    class ServiceState {
    public:
        enum stateT { MISSING_DEPENDENCIES = 0,
            UNINITIALIZED,
            RUNNING,
            STOPPED,
            FINISHED,
            STAND_BY,
            UNKNOWN };

        //    protected:
        const stateT m_state;

        virtual void somethingIsMissing() { }
        virtual void allFine() { }
        [[nodiscard]] auto getState() const -> stateT;

        explicit ServiceState(stateT state)
            : m_state(state)
        {
        }
    };

    class ProxyConfigs {
    public:
        struct Dependencie {
            ServiceState::stateT m_reqrState;
            ServiceState::stateT m_currState;
            Dependencie(ServiceState::stateT reqrState, ServiceState::stateT currState)
                : m_reqrState(reqrState)
                , m_currState(currState)
            {
            }

            Dependencie() = default;
        };

        std::map<std::string, Dependencie> m_depsMap;

        void changeDep(std::string dependencieId, ServiceState::stateT currState);
        explicit ProxyConfigs(std::map<std::string, ServiceState::stateT> depsMap);
    };

    proxyT m_proxyType;
    std::mutex m_statusMtx;
    ProxyConfigs m_proxyConfigs;
    std::string m_realServiceId;
    IService& m_realService;

    virtual void changeState(ServiceState::stateT newState) { }
    virtual void autoUpdate() { }
    virtual void serviceCycle() { }

public:
    virtual auto reportState() -> nlohmann::json { return { {} }; }

    ServiceProxy(IService& realService, proxyT proxyType, std::map<std::string, ServiceState::stateT> depsMap)
        : m_realService(realService)
        , m_proxyType(proxyType)
        , m_proxyConfigs(depsMap)
        , m_realServiceId(realService.m_serviceId)
    {
    }

    virtual ~ServiceProxy() = default;
};

