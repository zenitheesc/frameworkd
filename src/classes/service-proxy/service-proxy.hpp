#pragma once
#include "../service/service.hpp"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

class ServiceProxy;

class Status {
public:
    enum statusT { MISSING_DEPENDENCIES = 0,
        UNINITIALIZED,
        RUNNING,
        STOPPED,
        FINISHED,
        STAND_BY,
        UNKNOWN };

protected:
    statusT m_state;
    ServiceProxy* m_upperProxy;
    std::mutex m_statusMtx;

    virtual void someThingIsMissing() { }
    virtual void allFine() { }
    auto getStatus() -> statusT;
};

class ServiceProxy {
public:
    enum proxyT { staticService = 0,
        routineService };

protected:
    class ProxyConfigs {
        struct Dependencie {
            Status::statusT m_reqrState;
            Status::statusT m_currState;
        };

        std::map<std::string, Dependencie> m_depsMap;

    public:
        void changeDep(std::string dependencieId, Status::statusT currState);
        explicit ProxyConfigs(std::map<std::string, Status::statusT> depsMap);
    };

    proxyT m_proxyType;
    ProxyConfigs m_proxyConfigs;
    Status* m_state;
    IService& m_realService;

    virtual void serviceCycle() { }
    virtual void changeState() { }
    virtual void autoUpdate() { }
    auto reportStatus() -> nlohmann::json;

    explicit ServiceProxy(IService& realService, std::map<std::string, Status::statusT> depsMap);
};

namespace staticService {
class Proxy;

class Proxy : public ServiceProxy {
public:
};
}

namespace routineService {
class Proxy : public ServiceProxy {
public:
};
}
