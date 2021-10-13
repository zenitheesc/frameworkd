#include "service-proxy.hpp"

auto Status::getState() -> Status::stateT
{

    const std::lock_guard<std::mutex> lock(m_mtx);

    return m_state;
}

void Status::setState(Status::stateT newState)
{

    const std::lock_guard<std::mutex> lock(m_mtx);

    m_state = newState;
}

void ServiceProxy::servicePod()
{

    m_realService.setup();
    m_status.setState(Status::RUNNING);

    while (m_status.getState() != Status::STOPPED) {

        m_realService.routine();
    }

    m_realService.destroy();
}

ServiceProxy::ServiceProxy(IService& service, nlohmann::json configs)
    : m_realService(service)
    , m_dependencies(configs)
{
    m_status.setState(Status::UNINITIALIZED);
    if (configs["dependencies"] == 0) {
        run();
    }
}

ServiceProxy::~ServiceProxy()
{
    auto currState = m_status.getState();
    if (currState == Status::RUNNING) {
        stop();
    }
}

void ServiceProxy::run()
{
    std::thread thread(&ServiceProxy::servicePod, this);
    m_status.setState(Status::INITIALIZED);
    std::swap(thread, m_innerThread);
}

void ServiceProxy::stop()
{
    m_status.setState(Status::STOPPED);
    m_innerThread.join();
    m_status.setState(Status::DEAD);
}

auto ServiceProxy::getStatus() -> nlohmann::json
{
    auto currStatus = m_status.getState();
    std::lock_guard<std::mutex> lock(m_dependencies.m_mtx);

    return (nlohmann::json) { { "serviceId", m_dependencies.m_data["serviceId"] }, { "State", currStatus } };
}

