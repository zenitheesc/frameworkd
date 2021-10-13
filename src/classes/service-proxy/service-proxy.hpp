#pragma once
#include "../service/service.hpp"
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

class Status {
public:
    /*enum for the possible service-thread states*/
    enum stateT {
        MISSINGDEPENDENCIES = 0,
        UNINITIALIZED,
        INITIALIZED,
        RUNNING,
        STOPPED,
        DEAD
    };

private:
    /*state from @ref stateT enum*/
    stateT m_state;

    /*class' mutex to ensure it's thread safe*/
    std::mutex m_mtx;

public:
    /**
 	 * @brief	thread-safe getter to inside member @ref m_state
 	 * 
 	 * @return	stateT state that's inside the class.
 	 */
    auto getState() -> stateT;

    /**
	 * @brief	thread-safe setter to the inside member @ref m_state
	 *
	 * @param	stateT state that will be the new state of the class.
	 */
    void setState(stateT newState);
};

class ServiceProxy {
private:
    /* a reference for a Service implemented by the user */
    IService& m_innerService;

    /* structure made to allow the use of the service-proxy 
	 * conf-file in multiples threads.
	 */
    struct SafeJson {
        nlohmann::json data;
        std::mutex mtx;
    };

    /* json thread-safe that will contain the service-proxy conf-file */
    SafeJson m_dependencies;

    /* class made to keep track of innerThread current state */
    Status m_status;

    /* thread object that will contain the running thread */
    std::thread m_innerThread;

public:
    /**
	 * @brief	returns a json with the general status + information 
	 * 			about the service-proxy's innerThread and its innerService.
	 */
    auto getStatus() -> nlohmann::json;

    /* Function that will update the inner dependencies along changes in their states */
    //    nlohmann::json update(void);

    /**
	 * @brief	class' constructor
	 *
	 * @param	service IService class passed by reference
	 *
	 * @param	configs nlohmann::json that contains the configure
	 * 			files that refer to the service-proxy and 
	 * 			its @ref innerService
	 */
    ServiceProxy(IService& service, nlohmann::json configs);

    /**
	 * @brief	class' destructor
	 */
    ~ServiceProxy();

    //private:

    /**
 	 * @brief	starts running the @ref m_innerService in 
 	 * 			a thread that will be in @ref m_innerThread, 
	 * 			and that is encapsulated with @ref servicePod.
 	 */
    void run();

    /**
 	 * @brief	stops a running thread "located" in 
	 * 			@ref m_innerThread that contains an @ref m_innerService
	 * 			and that is encapsulated with @ref servicePod 
 	 */
    void stop();

private:
    /**
 	 * @brief	function that encapsulates the class @ref IService
	 * 			to be runned in a thread as explicit in @ref run
 	 */
    void servicePod(IService& service, Status& status);
};

