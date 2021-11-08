#include "routine-service-proxy.hpp"

RoutineServiceProxy::RoutineServiceProxy(IService& realService, std::map<std::string, RoutineState::stateT> depsMap)
    : ServiceProxy(realService, ROUTINE_SERVICE, depsMap)
{
    changeState(RoutineState::MISSING_DEPENDENCIES);
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
    changeState(RoutineState::RUNNING);
    m_realService.setup();

    while (checkState() != RoutineState::STOPPED) {

        m_realService.routine();
    }

    m_realService.destroy();

    changeState(RoutineState::FINISHED);
}

void RoutineServiceProxy::weave()
{
    const std::lock_guard<std::mutex> lock(m_updateMtx);
    changeState(RoutineState::STAND_BY);
    std::thread thread(&RoutineServiceProxy::serviceCycle, this);
    std::swap(thread, m_thread);
}

void RoutineServiceProxy::cut()
{
    const std::lock_guard<std::mutex> lock(m_updateMtx);
    changeState(RoutineState::STOPPED);
    m_thread.join();
    changeState(RoutineState::MISSING_DEPENDENCIES);
}

void RoutineServiceProxy::autoUpdate()
{
    bool noMissingDependencies = true;

    for (auto& [depId, dependency] : m_proxyConfigs.m_depsMap) {
        if (dependency.m_currState != dependency.m_reqrState) {
            noMissingDependencies = false;
        }
    }

    (noMissingDependencies) ? m_status->allFine() : m_status->somethingIsMissing();
}

void RoutineServiceProxy::changeState(RoutineState::stateT newState)
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);

    switch (newState) {
    case RoutineState::MISSING_DEPENDENCIES:
        m_status = std::make_unique<MissingDependencies>(*this);
        break;
    case RoutineState::RUNNING:
        m_status = std::make_unique<Running>(*this);
        break;
    case RoutineState::STAND_BY:
        m_status = std::make_unique<StandBy>(*this);
        break;
    case RoutineState::STOPPED:
        m_status = std::make_unique<Stopped>(*this);
        break;
    case RoutineState::FINISHED:
        m_status = std::make_unique<Finished>(*this);
        break;
    default:
        std::cout << "ERROR!! This was not supose to happen!! - Routine Service" << std::endl;
    }
}

auto RoutineServiceProxy::checkState() -> RoutineState::stateT
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);
    return m_status->getState();
}

auto RoutineServiceProxy::reportState() -> nlohmann::json
{
    RoutineState::stateT currStatus = checkState();

    return (nlohmann::json) { { "serviceId", m_realServiceId }, { "State", currStatus } };
}

