#pragma once
#include "../service-proxy/service-proxy.hpp"
#include <memory>

class RoutineServiceProxy : public ServiceProxy {
public:
    RoutineServiceProxy(IService& realService, std::map<std::string, ServiceState::state_t> depsMap);
    ~RoutineServiceProxy() override = default;

protected:
    friend class ServiceHandler;
    friend class Tester;

    class RoutineState : public ServiceState {

    public:
        RoutineServiceProxy& m_upperProxy;

        RoutineState(state_t state, RoutineServiceProxy& upperProxy)
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
    std::mutex m_updateMtx;

    void serviceCycle() override;
    void changeState(ServiceState::state_t newState) override;
    void weave();
    void cut();
};

