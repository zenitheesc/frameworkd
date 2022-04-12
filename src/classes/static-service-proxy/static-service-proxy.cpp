#include "frameworkd/classes/static-service-proxy/static-service-proxy.hpp"

StaticServiceProxy::StaticServiceProxy(StaticService& realService, std::map<std::string, ServiceState::state_t> depsMap)
    : m_runnedOnce { false }
    , ServiceProxy { realService, STATIC_SERVICE, depsMap }
{
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

/*
 * @brief:  Exposes the static-service on the DBUS and then
 *          changes m_upperProxy's StaticState to state 
 *          StandBy if the static-service already had run
 *          once, or to state Uninitialized if its doesn't.
 */
void StaticServiceProxy::MissingDependencies::allFine()
{
    state_t newState = (m_upperProxy.m_runnedOnce) ? STAND_BY : UNINITIALIZED;

    m_upperProxy.changeState(newState);
}

/*
 * @brief:  Hides the static-service from the DBUS and then
 *          changes m_upperProxy's StaticState to state 
 *          MissingDependencies.
 */
void StaticServiceProxy::Uninitialized::somethingIsMissing()
{
    m_upperProxy.changeState(ServiceState::MISSING_DEPENDENCIES);
}

/*
 * @brief:  Hides the static-service from the DBUS and then
 *          changes m_upperProxy's StaticState to state 
 *          MissingDependencies.
 */
void StaticServiceProxy::StandBy::somethingIsMissing()
{
    m_upperProxy.changeState(ServiceState::MISSING_DEPENDENCIES);
}

void StaticServiceProxy::StaticServiceProxy::serviceCycle()
{
    changeState(ServiceState::RUNNING);
    m_realService.destroy();
    changeState(ServiceState::STAND_BY);
}

void StaticServiceProxy::StaticServiceProxy::changeState(ServiceState::state_t newState)
{
    const std::lock_guard<std::mutex> lock { m_stateMtx };

    switch (newState) {
    case ServiceState::MISSING_DEPENDENCIES:
        m_state = std::make_unique<MissingDependencies>(*this);
        break;
    case ServiceState::UNINITIALIZED:
        m_state = std::make_unique<Uninitialized>(*this);
        break;
    case ServiceState::RUNNING:
        m_state = std::make_unique<Running>(*this);
        break;
    case ServiceState::STAND_BY:
        m_state = std::make_unique<StandBy>(*this);
        break;
    default:
        std::string errorMessage = "Unknown State! State Read = ";
        errorMessage.append(std::to_string(newState));
        throw std::logic_error(errorMessage);
    }
}

