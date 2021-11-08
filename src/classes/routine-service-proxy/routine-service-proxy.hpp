#pragma once
#include "../service-proxy/service-proxy.hpp"
#include <memory>

class RoutineServiceProxy : public ServiceProxy {
protected:
    friend class ServiceHandler;
    friend class Tester;

    class RoutineState : public ServiceState {

    public:
        RoutineServiceProxy& m_upperProxy;

        RoutineState(stateT state, RoutineServiceProxy& upperProxy)
            : m_upperProxy(upperProxy)
            , ServiceState(state)
        {
        }
    };

    class MissingDependencies : public RoutineState {
    public:
        explicit MissingDependencies(RoutineServiceProxy& upperProxy)
            : RoutineState(MISSING_DEPENDENCIES, upperProxy)
        {
        }

        void allFine() override;
    };

    class StandBy : public RoutineState {
    public:
        explicit StandBy(RoutineServiceProxy& upperProxy)
            : RoutineState(STAND_BY, upperProxy)
        {
        }
    };

    class Running : public RoutineState {
    public:
        explicit Running(RoutineServiceProxy& upperProxy)
            : RoutineState(RUNNING, upperProxy)
        {
        }

        void somethingIsMissing() override;
    };

    class Stopped : public RoutineState {
    public:
        explicit Stopped(RoutineServiceProxy& upperProxy)
            : RoutineState(STOPPED, upperProxy)
        {
        }
    };

    class Finished : public RoutineState {
    public:
        explicit Finished(RoutineServiceProxy& upperProxy)
            : RoutineState(FINISHED, upperProxy)
        {
        }
    };

    std::thread m_thread;
    std::unique_ptr<RoutineState> m_status;
    std::mutex m_updateMtx;

    void serviceCycle() override;
    void autoUpdate() override;
    void changeState(ServiceState::stateT newState) override;
    auto checkState() -> ServiceState::stateT;
    void weave();
    void cut();

public:
    RoutineServiceProxy(IService& realService, std::map<std::string, ServiceState::stateT> depsMap);
    ~RoutineServiceProxy() override = default;

    auto reportState() -> nlohmann::json override;
};

