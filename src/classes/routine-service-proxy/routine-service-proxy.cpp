#include "frameworkd/classes/routine-service-proxy/routine-service-proxy.hpp"

RoutineServiceProxy::RoutineServiceProxy(RoutineService& realService, std::map<std::string, ServiceState::state_t> depsMap)
    : ServiceProxy { realService, ROUTINE_SERVICE, depsMap }
{
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

/**
 * @brief:  Exposes the routine-service from the DBUS and then 
 *          calls m_upperProxy's method weave.
 */
void RoutineServiceProxy::MissingDependencies::allFine()
{
    m_upperProxy.weave();
}

/**
 * @brief:  Hides the routine-service from the DBUS and then calls 
 *          the m_upperProxy's method cut.
 */
void RoutineServiceProxy::Running::somethingIsMissing()
{
    m_upperProxy.cut();
}

void RoutineServiceProxy::serviceCycle()
{
    changeState(ServiceState::RUNNING);

    while (checkState() != ServiceState::STOPPED) {

        m_realService.routine();
    }

    m_realService.destroy();

    changeState(ServiceState::FINISHED);
}

void RoutineServiceProxy::weave()
{
    const std::lock_guard<std::mutex> lock { m_updateMtx };
    changeState(ServiceState::STAND_BY);
    std::thread thread(&RoutineServiceProxy::serviceCycle, this);
    std::swap(thread, m_thread);
}

void RoutineServiceProxy::cut()
{
    const std::lock_guard<std::mutex> lock { m_updateMtx };
    changeState(ServiceState::STOPPED);
    m_thread.join();
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

void RoutineServiceProxy::changeState(ServiceState::state_t newState)
{
    const std::lock_guard<std::mutex> lock { m_stateMtx };

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
        std::string errorMessage = "Unknown State! State Read = ";
        errorMessage.append(std::to_string(newState));
        throw std::logic_error(errorMessage);
    }
}

