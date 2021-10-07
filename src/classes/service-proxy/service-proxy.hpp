#pragma once
#include "../service/service.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>
#include <memory>

class Status {
public:
    enum stateT {
        MISSINGDEPENDENCIES = 0,
        UNINITIALIZED,
        INITIALIZED,
        RUNNING,
        STOPED,
        DEAD
    };

private:
    stateT m_state;
    std::mutex m_mtx;

public:
	auto getState()->stateT;
	void setState(stateT newState);
};


class ServiceProxy {
private:
	IService& m_innerService;
    struct SafeJson {
        nlohmann::json data;
        std::mutex mtx;
    };
    SafeJson m_dependencies;
    Status m_status;
    std::thread m_innerThread;

public:
    auto getStatus()->nlohmann::json;
    //    nlohmann::json update(void);
	ServiceProxy(IService& service, nlohmann::json configs);
    ~ServiceProxy();

//private:
    void run();
    void stop();
private:    
	static void servicePod(IService& service, Status& status);
};

