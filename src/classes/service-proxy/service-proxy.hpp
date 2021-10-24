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
    std::mutex m_statusMtx;

    virtual void somethingIsMissing() { }
    virtual void allFine() { }
    auto getState() -> stateT;
    explicit Status(stateT state)
        : m_state(state)
    {
    }
};

class ServiceProxy {
public:
    enum proxyT { staticService = 0,
        routineService };

protected:
    class ProxyConfigs {
        struct Dependencie {
            Status::stateT m_reqrState;
            Status::stateT m_currState;
        };

        std::map<std::string, Dependencie> m_depsMap;

    public:
        void changeDep(std::string dependencieId, Status::stateT currState);
        explicit ProxyConfigs(std::map<std::string, Status::stateT> depsMap);
    };

    proxyT m_proxyType;
    ProxyConfigs m_proxyConfigs;
    IService& m_realService;

    virtual void serviceCycle() { }
    virtual void changeState() { }
    virtual void autoUpdate() { }
    auto reportStatus() -> nlohmann::json { return { {} }; }

    explicit ServiceProxy(IService& realService, std::map<std::string, Status::stateT> depsMap);
};

namespace staticService {
class Proxy;

class SStatus : public Status {
protected:
    const Proxy* m_upperProxy;

public:
    explicit SStatus(stateT state)
        : Status(state)
    {
    }
};

class MissingDependencies : public SStatus {
public:
    explicit MissingDependencies()
        : SStatus(MISSING_DEPENDENCIES)
    {
    }
    void allFine() override;
};

class Uninitialized : public SStatus {
public:
    explicit Uninitialized()
        : SStatus(UNINITIALIZED)
    {
    }
    void somethingIsMissing() override;
};

class Running : public SStatus {
public:
    explicit Running()
        : SStatus(RUNNING)
    {
    }
};

class StandBy : public SStatus {
public:
    explicit StandBy()
        : SStatus(STAND_BY)
    {
    }
    void somethingIsMissing() override;
};

class Proxy : public ServiceProxy {
protected:
    bool m_runnedOnce;
    SStatus* m_status;

public:
    void execute() { serviceCycle(); } // Temporary!!!
};
} // namespace staticService

namespace routineService {
class Proxy;
class RStatus : public Status {
public:
    std::mutex m_borrowMtx;
    Proxy* m_upperProxy;

    explicit RStatus(stateT state)
        : Status(state)
    {
    }
};

class MissingDependencies : public RStatus {
public:
    explicit MissingDependencies()
        : RStatus(MISSING_DEPENDENCIES)
    {
    }

    void allFine() override;
};

class StandBy : public RStatus {
public:
    explicit StandBy()
        : RStatus(STAND_BY)
    {
    }
};

class Running : public RStatus {
public:
    explicit Running()
        : RStatus(RUNNING)
    {
    }
    
    void somethingIsMissing() override;
};

class Stopped : public RStatus {
public:
    explicit Stopped()
        : RStatus(STOPPED)
    {
    }
};

class Finished : public RStatus {
public:
    explicit Finished()
        : RStatus(FINISHED)
    {
    }
};

class Proxy : public ServiceProxy {
protected:
    std::thread m_thread;
    void weave();
    void cut();
};
} // namespace routineService

