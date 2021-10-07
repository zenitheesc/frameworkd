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

void ServiceProxy::servicePod(IService& service, Status& status)
{

    service.setup();
    status.setState(Status::RUNNING);

    while (status.getState() != Status::STOPED) {

        service.routine();
    }

    service.destroy();
}

ServiceProxy::ServiceProxy(IService& service, nlohmann::json configs)
    : m_innerService(service)
{
    m_dependencies.data = configs; //DETAILS ABOUT DEPENDENCIES

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
    std::thread thread(servicePod, std::ref(m_innerService), std::ref(m_status));
    m_status.setState(Status::INITIALIZED);
    std::swap(thread, m_innerThread);
}

void ServiceProxy::stop()
{
    m_status.setState(Status::STOPED);
    m_innerThread.join();
    m_status.setState(Status::DEAD);
}

auto ServiceProxy::getStatus() -> nlohmann::json
{
    auto currStatus = m_status.getState();
    std::lock_guard<std::mutex> lock(m_dependencies.mtx);

    return (nlohmann::json) { { "serviceId", m_dependencies.data["serviceId"] }, { "State", currStatus } };
}

