#include "static-service-proxy.hpp"

void StaticServiceProxy::MissingDependencies::allFine()
{
    stateT newState = (m_upperProxy.m_runnedOnce) ? STAND_BY : UNINITIALIZED;

    m_upperProxy.changeState(newState);
}

// Uninitialized and StandBy Constructor will expose the endpoint on the DBUS

void StaticServiceProxy::Uninitialized::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy.changeState(MISSING_DEPENDENCIES);
}

void StaticServiceProxy::StandBy::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN...
    m_upperProxy.changeState(MISSING_DEPENDENCIES);
}

StaticServiceProxy::StaticServiceProxy(IService& realService, std::map<std::string, ServiceState::stateT> depsMap)
    : m_runnedOnce(false)
    , ServiceProxy(realService, STATIC_SERVICE, depsMap)
{
    changeState(ServiceState::MISSING_DEPENDENCIES);
}

void StaticServiceProxy::StaticServiceProxy::autoUpdate()
{
    bool noMissingDependencies = true;

    for (auto& [depId, dependency] : m_proxyConfigs.m_depsMap) {
        if (dependency.m_currState != dependency.m_reqrState) {
            noMissingDependencies = false;
        }
    }

    (noMissingDependencies) ? m_status->allFine() : m_status->somethingIsMissing();
}

void StaticServiceProxy::StaticServiceProxy::serviceCycle()
{
    changeState(ServiceState::RUNNING);
    m_realService.setup();
    m_realService.destroy();
    changeState(ServiceState::STAND_BY);
}

void StaticServiceProxy::StaticServiceProxy::changeState(ServiceState::stateT newState)
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);

    switch (newState) {
    case ServiceState::MISSING_DEPENDENCIES:
        m_status = std::make_unique<MissingDependencies>(*this);
        break;
    case ServiceState::UNINITIALIZED:
        m_status = std::make_unique<Uninitialized>(*this);
        break;
    case ServiceState::RUNNING:
        m_status = std::make_unique<Running>(*this);
        break;
    case ServiceState::STAND_BY:
        m_status = std::make_unique<StandBy>(*this);
        break;
    default:
        std::cout << "ERROR!! This was not supposed to happen!! - Static Service" << std::endl;
    }
}

auto StaticServiceProxy::StaticServiceProxy::checkState() -> ServiceState::stateT
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);
    return m_status->getState();
}

auto StaticServiceProxy::StaticServiceProxy::reportState() -> nlohmann::json
{
    ServiceState::stateT currStatus = checkState();

    return (nlohmann::json) { { "serviceId", m_realServiceId }, { "State", currStatus } };
}
