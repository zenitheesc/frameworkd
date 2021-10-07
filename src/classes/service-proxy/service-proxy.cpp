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

void ServiceProxy::servicePod(std::unique_ptr<IService> service, Status& status)
{

    service->setup();
    status.setState(Status::RUNNING);

    while (status.getState() == Status::STOPED) {

        service->routine();
    }

    service->destroy();
}

ServiceProxy::ServiceProxy(IService& service, nlohmann::json configs)
    : m_innerService(std::make_unique<IService>(service))
{
    m_dependencies.data = configs; //DETAILS ABOUT DEPENDENCIES
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
    std::thread thread(servicePod, std::ref(m_innerService), std::ref(m_status)); //WARNINGGG!!!!!!!!!!!!!!!!!!!
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

    return { m_dependencies.data["serviceId"], { "State", currStatus } };
}

