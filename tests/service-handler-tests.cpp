#include "../src/classes/iservice/iservice.hpp"
#include "../src/classes/routine-service-proxy/routine-service-proxy.hpp"
#include "../src/classes/service-handler/service-handler.hpp"
#include "../src/classes/service-proxy/service-proxy.hpp"
#include "../src/classes/static-service-proxy/static-service-proxy.hpp"

#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

std::mutex sharedDataMtx;

class ServiceTest : public ::testing::Test {
protected:
    struct SharedData {
        bool staticServiceFlag { false };
        int routineServiceCounter { 0 };
    };

    class MyService2 : public StaticService {
    public:
        explicit MyService2(std::string id)
            : StaticService(id) {};

        int coisa1;
        int coisa2;

        void setup() override
        {
            coisa1 = 1;
            coisa2 = 4;
        }

        void destroy() override
        {
            std::lock_guard<std::mutex> lock(sharedDataMtx);
            std::cout << "ENDPOINT HERE" << std::endl;
        }
    };

    class MyService : public RoutineService {
    public:
        explicit MyService(std::string id)
            : RoutineService(id) {};

        int coisa1;
        int coisa2;

        void setup() override
        {
            coisa1 = 1;
            coisa2 = 4;
        }

        void routine() override
        {
            std::lock_guard<std::mutex> lock(sharedDataMtx);
            std::cout << "Wooow!! Looop!!" << std::endl;
            std::cout << "coisa1: " << coisa1 << std::endl;
            std::cout << "coisa2: " << coisa2 << std::endl;
        }

        void destroy() override
        {
            std::lock_guard<std::mutex> lock(sharedDataMtx);
        }
    };
    nlohmann::json m_handler1_config;
    ServiceTest()
        : m_handler1_config { { "proxys", { { "NotMyService", { { "Ghost", ServiceProxy::ServiceState::RUNNING }, { "Batata", ServiceProxy::ServiceState::MISSING_DEPENDENCIES } } }, { "MyService", {} } } } }
        , m_testHandler { m_handler1_config } {};

    ServiceHandler m_testHandler;
};

TEST_F(ServiceTest, RequestOneProxyState)
{
    MyService2 m_service2("MyService");
    m_testHandler.buildServiceProxy(m_service2);
    nlohmann::json expectedOutput = { { "State", ServiceProxy::ServiceState::MISSING_DEPENDENCIES }, { "serviceId", "MyService" } };
    EXPECT_EQ(m_testHandler.getProxyState("MyService"), expectedOutput);
    //testHandler.run();
    //EXPECT_EQ(testHandler.getProxyState("MyService"), ServiceProxy::ServiceState::UNINITIALIZED);
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
