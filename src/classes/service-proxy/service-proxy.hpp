#pragma once
#include "../service/service.hpp"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

class ServiceProxy;

class Status {
public:
    enum stateT { MISSING_DEPENDENCIES = 0,
        UNINITIALIZED,
        RUNNING,
        STOPPED,
        FINISHED,
        STAND_BY,
        UNKNOWN };

protected:
    const stateT m_state;
    std::mutex* const m_statusMtx;

    virtual void somethingIsMissing() { }
    virtual void allFine() { }
    auto getState() -> stateT;
    Status(stateT state, std::mutex* const statusMtx)
        : m_state(state), m_statusMtx(statusMtx)
    {
    }
};

class ServiceProxy {
public:
    enum proxyT { STATIC_SERVICE = 0,
        ROUTINE_SERVICE };

protected:
    class ProxyConfigs {
    public:
        struct Dependencie {
            Status::stateT m_reqrState;
            Status::stateT m_currState;
            Dependencie(Status::stateT reqrState, Status::stateT currState)
                : m_reqrState(reqrState)
                , m_currState(currState)
            {
            }

            Dependencie() = default;
        };

        std::map<std::string, Dependencie> m_depsMap;

        void changeDep(std::string dependencieId, Status::stateT currState);
        explicit ProxyConfigs(std::map<std::string, Status::stateT> depsMap);
    };

    proxyT m_proxyType;
    std::mutex m_statusMtx;
    ProxyConfigs m_proxyConfigs;
    std::string m_realServiceId;
    IService& m_realService;

    virtual void serviceCycle() { }
    virtual void changeState(Status::stateT newState) { }
    virtual void autoUpdate() { }
    virtual auto reportStatus() -> nlohmann::json { return { {} }; }

    ServiceProxy(IService& realService, proxyT proxyType, std::map<std::string, Status::stateT> depsMap)
        : m_realService(realService)
        , m_proxyType(proxyType)
        , m_proxyConfigs(depsMap),
        m_realServiceId(realService.m_serviceId)
    {
    }
};

namespace staticService {
class Proxy;

class SStatus : public Status {
    friend class Proxy;

protected:
    Proxy* const m_upperProxy;

public:
    SStatus(stateT state, Proxy* const upperProxy, std::mutex* const statusMtx)
        : Status(state, statusMtx)
        , m_upperProxy(upperProxy)
    {
    }
};

class MissingDependencies : public SStatus {
public:
    explicit MissingDependencies(Proxy* const upperProxy, std::mutex* const statusMtx)
        : SStatus(MISSING_DEPENDENCIES, upperProxy, statusMtx)
    {
    }
    void allFine() override;
};

class Uninitialized : public SStatus {
public:
    explicit Uninitialized(Proxy* const upperProxy, std::mutex* const statusMtx)
        : SStatus(UNINITIALIZED, upperProxy, statusMtx)
    {
    }
    void somethingIsMissing() override;
};

class Running : public SStatus {
public:
    explicit Running(Proxy* const upperProxy, std::mutex* const statusMtx)
        : SStatus(RUNNING, upperProxy, statusMtx)
    {
    }
};

class StandBy : public SStatus {
public:
    explicit StandBy(Proxy* const upperProxy, std::mutex* const statusMtx)
        : SStatus(STAND_BY, upperProxy, statusMtx)
    {
    }
    void somethingIsMissing() override;
};

class Proxy : public ServiceProxy {
    friend class MissingDependencies;
    friend class Uninitialized;
    friend class Running;
    friend class StandBy;

protected:
    bool m_runnedOnce;
    SStatus* m_status;
    void changeState(Status::stateT newState) override;
    void serviceCycle() override;
    void autoUpdate() override;
    auto checkState() -> Status::stateT;
    Proxy(IService& realService, std::map<std::string, Status::stateT> depsMap);
    ~Proxy();


public:
    auto reportStatus()-> nlohmann::json override;
    void execute() { serviceCycle(); } // Temporary!!!
};
} // namespace staticService

namespace routineService {
class Proxy;
class RStatus : public Status {
    friend class Proxy;

public:
    std::mutex* const m_updateMtx;
    Proxy* const m_upperProxy;

    RStatus(stateT state, Proxy* const upperProxy, std::mutex* const statusMtx, std::mutex* const updateMtx)
        : m_upperProxy(upperProxy),
        m_updateMtx(updateMtx)
        , Status(state, statusMtx)
    {
    }
};

class MissingDependencies : public RStatus {
public:
    explicit MissingDependencies(Proxy* const upperProxy, std::mutex* const statusMtx, std::mutex* const updateMtx)
        : RStatus(MISSING_DEPENDENCIES, upperProxy, statusMtx, updateMtx)
    {
    }

    void allFine() override;
};

class StandBy : public RStatus {
public:
    explicit StandBy(Proxy* const upperProxy, std::mutex* const statusMtx, std::mutex* const updateMtx)
        : RStatus(STAND_BY, upperProxy, statusMtx, updateMtx)
    {
    }
};

class Running : public RStatus {
public:
    explicit Running(Proxy* const upperProxy, std::mutex* const statusMtx, std::mutex* const updateMtx)
        : RStatus(RUNNING, upperProxy, statusMtx, updateMtx)
    {
    }

    void somethingIsMissing() override;
};

class Stopped : public RStatus {
public:
    explicit Stopped(Proxy* const upperProxy, std::mutex* const statusMtx, std::mutex* const updateMtx)
        : RStatus(STOPPED, upperProxy, statusMtx, updateMtx)
    {
    }
};

class Finished : public RStatus {
public:
    explicit Finished(Proxy* const upperProxy, std::mutex* const statusMtx, std::mutex* const updateMtx)
        : RStatus(FINISHED, upperProxy, statusMtx, updateMtx)
    {
    }
};

class Proxy : public ServiceProxy {
    friend class MissingDependencies;
    friend class StandBy;
    friend class Running;
    friend class Stopped;
    friend class Finished;

protected:
    std::thread m_thread;
    RStatus* m_status;
    std::mutex m_updateMtx;

    void serviceCycle() override;
    void autoUpdate() override;
    void changeState(Status::stateT newState) override;
    auto checkState() -> Status::stateT;
    void weave();
    void cut();
    Proxy(IService& realService, std::map<std::string, Status::stateT> depsMap);
    ~Proxy();
public:
    auto reportStatus()-> nlohmann::json override;
};
} // namespace routineService

