#include "routine-service-proxy.hpp"

RoutineServiceProxy::RoutineServiceProxy(RoutineService& realService, std::map<std::string, ServiceState::state_t> depsMap)
    : ServiceProxy(realService, ROUTINE_SERVICE, depsMap)
{
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

void RoutineServiceProxy::MissingDependencies::allFine()
{
    // Expose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy.weave();
}

void RoutineServiceProxy::Running::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy.cut();
}

void RoutineServiceProxy::serviceCycle()
{
    changeState(ServiceState::RUNNING);
    m_realService.setup();

    while (checkState() != ServiceState::STOPPED) {

        m_realService.routine();
    }

    m_realService.destroy();

    changeState(ServiceState::FINISHED);
}

void RoutineServiceProxy::weave()
{
    const std::lock_guard<std::mutex> lock(m_updateMtx);
    changeState(ServiceState::STAND_BY);
    std::thread thread(&RoutineServiceProxy::serviceCycle, this);
    std::swap(thread, m_thread);
}

void RoutineServiceProxy::cut()
{
    const std::lock_guard<std::mutex> lock(m_updateMtx);
    changeState(ServiceState::STOPPED);
    m_thread.join();
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

void RoutineServiceProxy::changeState(ServiceState::state_t newState)
{
    const std::lock_guard<std::mutex> lock(m_stateMtx);

    switch (newState) {
    case ServiceState::MISSING_DEPENDENCIES:
        m_state = std::make_unique<MissingDependencies>(*this);
        break;
    case ServiceState::RUNNING:
        m_state = std::make_unique<Running>(*this);
        break;
    case ServiceState::STAND_BY:
        m_state = std::make_unique<StandBy>(*this);
        break;
    case ServiceState::STOPPED:
        m_state = std::make_unique<Stopped>(*this);
        break;
    case ServiceState::FINISHED:
        m_state = std::make_unique<Finished>(*this);
        break;
    default:
        throw std::logic_error("Routine Services should not have other States!");
    }
}

