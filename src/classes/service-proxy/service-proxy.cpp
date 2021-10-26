#include "service-proxy.hpp"
#include <iostream>
#include <iterator>

auto Status::getState() -> Status::stateT
{
    return m_state;
}

ServiceProxy::ProxyConfigs::ProxyConfigs(std::map<std::string, Status::stateT> depsMap)
{
    std::map<std::string, Status::stateT>::iterator itr;

    for (itr = depsMap.begin(); itr != depsMap.end(); itr++) {

        //m_depsMap.emplace(itr->first, Dependencie(itr->second, Status::UNKNOWN));
        m_depsMap.emplace(std::piecewise_construct,
            std::forward_as_tuple(itr->first),
            std::forward_as_tuple(itr->second, Status::UNKNOWN));
    }
}

void ServiceProxy::ProxyConfigs::changeDep(std::string dependencieId, Status::stateT currState)
{
    m_depsMap[dependencieId].m_currState = currState;
}

void staticService::MissingDependencies::allFine()
{

    stateT newState = UNINITIALIZED;
    if (m_upperProxy->m_runnedOnce) {

        newState = STAND_BY;
    }

    m_upperProxy->changeState(newState);
}

// Uninitialized and StandBy Constructor will expose the endpoint on the DBUS

void staticService::Uninitialized::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy->changeState(MISSING_DEPENDENCIES);
}

void staticService::StandBy::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN...
    m_upperProxy->changeState(MISSING_DEPENDENCIES);
}

staticService::Proxy::Proxy(IService& realService, std::map<std::string, Status::stateT> depsMap)
    : ServiceProxy(realService, STATIC_SERVICE, depsMap)
{
    changeState(Status::MISSING_DEPENDENCIES);
    autoUpdate();
}

staticService::Proxy::~Proxy()
{
    delete m_status;
}

void staticService::Proxy::autoUpdate()
{
    bool noMissingDependencies = true;

    std::map<std::string, ProxyConfigs::Dependencie>::iterator itr;

    for (itr = m_proxyConfigs.m_depsMap.begin(); itr != m_proxyConfigs.m_depsMap.end(); itr++) {
        if (itr->second.m_currState != itr->second.m_reqrState) {
            noMissingDependencies = false;
        }
    }

    (noMissingDependencies) ? m_status->allFine() : m_status->somethingIsMissing();
}

void staticService::Proxy::serviceCycle()
{
    changeState(Status::RUNNING);
    m_realService.setup();
    m_realService.destroy();
    changeState(Status::STAND_BY);
}

void staticService::Proxy::changeState(Status::stateT newState)
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);

    switch (newState) {
    case Status::MISSING_DEPENDENCIES:
        delete m_status;
        m_status = new MissingDependencies(this, &m_statusMtx);
        break;
    case Status::UNINITIALIZED:
        delete m_status;
        m_status = new Uninitialized(this, &m_statusMtx);
        break;
    case Status::RUNNING:
        delete m_status;
        m_status = new Running(this, &m_statusMtx);
        break;
    case Status::STAND_BY:
        delete m_status;
        m_status = new StandBy(this, &m_statusMtx);
        break;
    case Status::STOPPED:
    case Status::UNKNOWN:
    case Status::FINISHED:
        std::cout << "ERROR!! This was not supose to happen!! - Static Service" << std::endl;
    };
}

auto staticService::Proxy::checkState() -> Status::stateT
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);
    return m_status->getState();
}

auto staticService::Proxy::reportStatus() -> nlohmann::json
{
    Status::stateT currStatus = checkState();

    return (nlohmann::json) { { "serviceId", m_realServiceId }, { "State", currStatus } };
}

routineService::Proxy::Proxy(IService& realService, std::map<std::string, Status::stateT> depsMap)
    : ServiceProxy(realService, ROUTINE_SERVICE, depsMap)
{
    changeState(Status::MISSING_DEPENDENCIES);
    autoUpdate();
}

routineService::Proxy::~Proxy()
{
    delete m_status;
}

void routineService::MissingDependencies::allFine()
{
    // Expose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy->weave();
}

void routineService::Running::somethingIsMissing()
{
    // Unexpose/Hide the endpoint on the DBUS and THEN..
    m_upperProxy->cut();
}

void routineService::Proxy::serviceCycle()
{
    changeState(Status::RUNNING);
    m_realService.setup();

    while (checkState() != Status::STOPPED) {

        m_realService.routine();
    }

    m_realService.destroy();

    changeState(Status::FINISHED);
}

void routineService::Proxy::weave()
{
    const std::lock_guard<std::mutex> lock(m_updateMtx);
    changeState(Status::STAND_BY);
    std::thread thread(&routineService::Proxy::serviceCycle, this);
    std::swap(thread, m_thread);
}

void routineService::Proxy::cut()
{
    const std::lock_guard<std::mutex> lock(m_updateMtx);
    changeState(Status::STOPPED);
    m_thread.join();
    changeState(Status::MISSING_DEPENDENCIES);
}

void routineService::Proxy::autoUpdate()
{
    bool noMissingDependencies = true;

    std::map<std::string, ProxyConfigs::Dependencie>::iterator itr;

    for (itr = m_proxyConfigs.m_depsMap.begin(); itr != m_proxyConfigs.m_depsMap.end(); itr++) {
        if (itr->second.m_currState != itr->second.m_reqrState) {
            noMissingDependencies = false;
        }
    }

    (noMissingDependencies) ? m_status->allFine() : m_status->somethingIsMissing();
}

void routineService::Proxy::changeState(Status::stateT newState)
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);

    switch (newState) {
    case Status::MISSING_DEPENDENCIES:
        delete m_status;
        m_status = new MissingDependencies(this, &m_statusMtx, &m_updateMtx);
        break;
    case Status::RUNNING:
        delete m_status;
        m_status = new Running(this, &m_statusMtx, &m_updateMtx);
        break;
    case Status::STAND_BY:
        delete m_status;
        m_status = new StandBy(this, &m_statusMtx, &m_updateMtx);
        break;
    case Status::STOPPED:
        delete m_status;
        m_status = new Stopped(this, &m_statusMtx, &m_updateMtx);
        break;
    case Status::FINISHED:
        delete m_status;
        m_status = new Finished(this, &m_statusMtx, &m_updateMtx);
        break;
    case Status::UNINITIALIZED:
    case Status::UNKNOWN:
        std::cout << "ERROR!! This was not supose to happen!! - Routine Service" << std::endl;
    };
}

auto routineService::Proxy::checkState() -> Status::stateT
{
    const std::lock_guard<std::mutex> lock(m_statusMtx);
    return m_status->getState();
}

auto routineService::Proxy::reportStatus() -> nlohmann::json
{
    Status::stateT currStatus = checkState();

    return (nlohmann::json) { { "serviceId", m_realServiceId }, { "State", currStatus } };
}

