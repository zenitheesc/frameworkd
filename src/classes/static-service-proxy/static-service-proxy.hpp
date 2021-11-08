#pragma once
#include "../service-proxy/service-proxy.hpp"
#include <memory>

class StaticServiceProxy : public ServiceProxy {
protected:
    friend class ServiceHandler;
    friend class Tester;

    class StaticState : public ServiceState {
    protected:
        StaticServiceProxy& m_upperProxy;

    public:
        StaticState(stateT state, StaticServiceProxy& upperProxy)
            : ServiceState(state)
            , m_upperProxy(upperProxy)
        {
        }
    };

    class MissingDependencies : public StaticState {
    public:
        explicit MissingDependencies(StaticServiceProxy& upperProxy)
            : StaticState(MISSING_DEPENDENCIES, upperProxy)
        {
        }
        void allFine() override;
    };

    class Uninitialized : public StaticState {
    public:
        explicit Uninitialized(StaticServiceProxy& upperProxy)
            : StaticState(UNINITIALIZED, upperProxy)
        {
        }
        void somethingIsMissing() override;
    };

    class Running : public StaticState {
    public:
        explicit Running(StaticServiceProxy& upperProxy)
            : StaticState(RUNNING, upperProxy)
        {
        }
    };

    class StandBy : public StaticState {
    public:
        explicit StandBy(StaticServiceProxy& upperProxy)
            : StaticState(STAND_BY, upperProxy)
        {
        }
        void somethingIsMissing() override;
    };

    bool m_runnedOnce;
    std::unique_ptr<StaticState> m_status;

    void changeState(ServiceState::stateT newState) override;
    void serviceCycle() override;
    void autoUpdate() override;
    auto checkState() -> ServiceState::stateT;

public:
    StaticServiceProxy(IService& realService, std::map<std::string, ServiceState::stateT> depsMap);
    ~StaticServiceProxy() override = default;

    auto reportState() -> nlohmann::json override;
};

