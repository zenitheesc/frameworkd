#include "static-service-proxy.hpp"

StaticServiceProxy::StaticServiceProxy(StaticService& realService, std::map<std::string, ServiceState::state_t> depsMap)
    : m_runnedOnce(false)
    , ServiceProxy(realService, STATIC_SERVICE, depsMap)
{
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

void StaticServiceProxy::MissingDependencies::allFine()
{
    state_t newState = (m_upperProxy.m_runnedOnce) ? STAND_BY : UNINITIALIZED;

    m_upperProxy.changeState(newState);
}

// Uninitialized and StandBy Constructor will expose the endpoint on the DBUS

void StaticServiceProxy::Uninitialized::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy.changeState(ServiceState::MISSING_DEPENDENCIES);
}

void StaticServiceProxy::StandBy::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN...
    m_upperProxy.changeState(ServiceState::MISSING_DEPENDENCIES);
}

void StaticServiceProxy::StaticServiceProxy::serviceCycle()
{
    changeState(ServiceState::RUNNING);
    m_realService.setup();
    m_realService.destroy();
    changeState(ServiceState::STAND_BY);
}

void StaticServiceProxy::StaticServiceProxy::changeState(ServiceState::state_t newState)
{
    const std::lock_guard<std::mutex> lock(m_stateMtx);

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
        std::cout << "ERROR!! This was not supposed to happen!! - Static Service" << std::endl;
    }
}

