#include "../src/classes/iservice/iservice.hpp"
#include "../src/classes/service-handler/service-handler.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <thread>

struct SharedData {
    bool setupFlag { false };
    bool destroyFlag { false };
    int fooCounter { 0 };
    std::mutex m_mtx;
};

void reset(SharedData& data)
{
    data.destroyFlag = false;
    data.setupFlag = false;
    data.fooCounter = 0;
}

SharedData staticShared;
SharedData routineShared;

class ServiceHandlerTests : public ::testing::Test {
public:
protected:
    class Service1 : public StaticService {
    public:
        explicit Service1(std::string id)
            : StaticService { id }
            , m_attribute1 { 0 }
            , m_attribute2 { "DEFAULT" } {};

        int m_attribute1;
        std::string m_attribute2;

        void setup() override
        {
            std::lock_guard<std::mutex> lock { staticShared.m_mtx };
            staticShared.setupFlag = true;

            m_attribute1++;
            if (m_attribute2 == "DEFAULT") {
                m_attribute1++;
            }

            staticShared.fooCounter++;
        }

        void destroy() override
        {
            std::lock_guard<std::mutex> lock { staticShared.m_mtx };
            staticShared.destroyFlag = true;
            staticShared.fooCounter++;
        }
    };

    class Service2 : public RoutineService {
    public:
        explicit Service2(std::string id)
            : RoutineService { id }
            , m_attribute1 { 0 }
            , m_attribute2 { "DEFAULT" } {};

        int m_attribute1;
        std::string m_attribute2;

        void setup() override
        {
            std::lock_guard<std::mutex> lock { routineShared.m_mtx };
            routineShared.setupFlag = true;

            m_attribute1++;
            if (m_attribute2 == "DEFAULT") {
                m_attribute1++;
            }
        }

        void routine() override
        {
            std::lock_guard<std::mutex> lock { routineShared.m_mtx };
            if (routineShared.fooCounter <= 2) {
                routineShared.fooCounter++;
            }
        }

        void destroy() override
        {
            std::lock_guard<std::mutex> lock { routineShared.m_mtx };
            routineShared.destroyFlag = true;
        }
    };
};

TEST_F(ServiceHandlerTests, BasicRequestProxyState)
{
    nlohmann::json configJSON = { { "proxys", { { "Camera", { { "Energy", ServiceProxy::ServiceState::RUNNING }, { "Emergency", ServiceProxy::ServiceState::MISSING_DEPENDENCIES } } }, { "Energy", {} } } } };

    ServiceHandler handler(configJSON);

    Service1 camera("Camera");

    handler.buildServiceProxy(camera);

    nlohmann::json expectedState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Camera" } };

    auto result = handler.getAllProxyState();
    EXPECT_EQ(result["Camera"], expectedState);

    result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expectedState);
}

TEST_F(ServiceHandlerTests, Run)
{
    nlohmann::json configJSON = { { "proxys", { { "Camera", { { "Energy", ServiceProxy::ServiceState::RUNNING }, { "Emergency", ServiceProxy::ServiceState::MISSING_DEPENDENCIES } } }, { "Energy", {} } } } };

    ServiceHandler handler(configJSON);

    Service1 energy("Energy");

    handler.buildServiceProxy(energy);

    nlohmann::json expectedState = { { "State", ServiceProxy::ServiceState::UNINITIALIZED }, { "serviceId", "Energy" } };

    handler.run();

    auto result = handler.getAllProxyState();

    EXPECT_EQ(result["Energy"], expectedState);
}

TEST_F(ServiceHandlerTests, FullRequestProxyState)
{
    nlohmann::json configJSON = { { "proxys", { { "Camera", { { "Radio", ServiceProxy::ServiceState::RUNNING }, { "TermicShield", ServiceProxy::ServiceState::MISSING_DEPENDENCIES } } }, { "Energy", {} } } } };

    ServiceHandler handler(configJSON);

    Service1 camera("Camera");

    Service1 energy("Energy");

    handler.buildServiceProxy(camera);
    handler.buildServiceProxy(energy);

    nlohmann::json expCameraState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Camera" } };
    nlohmann::json expEnergyState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Energy" } };

    auto result = handler.getAllProxyState();
    EXPECT_EQ(result["Camera"], expCameraState);
    EXPECT_EQ(result["Energy"], expEnergyState);

    result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expCameraState);

    result = handler.getProxyState("Energy");
    EXPECT_EQ(result, expEnergyState);

    handler.run();

    expEnergyState = { { "State", ServiceProxy::ServiceState::UNINITIALIZED }, { "serviceId", "Energy" } };

    result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expCameraState);
    EXPECT_EQ(routineShared.setupFlag, false);

    result = handler.getProxyState("Energy");
    EXPECT_EQ(result, expEnergyState);

    reset(routineShared);
}

TEST_F(ServiceHandlerTests, ChangeProxyDependency)
{
    nlohmann::json configJSON = { { "proxys", { { "Camera", { { "Radio", ServiceProxy::ServiceState::RUNNING }, { "TermicShield", ServiceProxy::ServiceState::MISSING_DEPENDENCIES } } }, { "Energy", {} } } } };

    ServiceHandler handler(configJSON);

    Service2 camera("Camera");

    Service1 energy("Energy");

    handler.buildServiceProxy(camera);
    handler.buildServiceProxy(energy);
    
    ASSERT_THROW(handler.changeDependencyState("Energy", "THIS", ServiceProxy::ServiceState::RUNNING), std::invalid_argument);

    handler.run();
    
    nlohmann::json expCameraState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Camera" } };
    handler.changeDependencyState("Camera", "TermicShield", ServiceProxy::ServiceState::MISSING_DEPENDENCIES);
    auto result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expCameraState);
    
    expCameraState = { { "State", ServiceProxy::ServiceState::RUNNING }, { "serviceId", "Camera" } };
    handler.changeDependencyState("Camera", "Radio", ServiceProxy::ServiceState::RUNNING);
    
    handler.updateServiceProxy("Camera");
 
    std::chrono::seconds fewSeconds(3);
    std::this_thread::sleep_for(fewSeconds); 
    
    handler.updateServiceProxy("Camera");
   
    result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expCameraState);

    

    handler.changeDependencyState("Camera", "Radio", ServiceProxy::ServiceState::MISSING_DEPENDENCIES);
    handler.updateServiceProxy("Camera");
    
    expCameraState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Camera" } };
    result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expCameraState);

    EXPECT_EQ(routineShared.setupFlag, true);
    EXPECT_EQ(routineShared.fooCounter, 3);
    EXPECT_EQ(routineShared.setupFlag, true);

    /*nlohmann::json expEnergyState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Energy" } };


    expEnergyState = { { "State", ServiceProxy::ServiceState::UNINITIALIZED }, { "serviceId", "Energy" } };
    expCameraState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Camera" } };
    expEnergyState = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "Energy" } };


    result = handler.getProxyState("Camera");
    EXPECT_EQ(result, expCameraState);
    EXPECT_EQ(routineShared.setupFlag, false);

    result = handler.getProxyState("Energy");
    EXPECT_EQ(result, expEnergyState);
*/
    reset(routineShared);

}

TEST_F(ServiceHandlerTests, UpdateProxys)
{
}

/*
 * public:
    auto getProxyState(std::string serviceId) -> nlohmann::json;
    auto getAllProxyState() -> nlohmann::json;
    void buildServiceProxy(StaticService& userService);
    void buildServiceProxy(RoutineService& userService);
    void changeDependencyState(std::string serviceId, std::string dependencyId, ServiceProxy::ServiceState::state_t newState);
    void updateServiceProxy(std::string serviceId);
    void updateAllServiceProxys();
    void run();
*/
