#include "../src/classes/iservice/iservice.hpp"
#include "../src/classes/service-handler/service-handler.hpp"

#include <gtest/gtest.h>

class ServiceTest : public ::testing::Test {
protected:
    class Service1 : public StaticService {
    public:
        explicit Service1(std::string id)
            : StaticService(id) {};

        void setup() override {};

        void destroy() override {};
    };

    class Service2 : public RoutineService {
    public:
        explicit Service2(std::string id)
            : RoutineService(id) {};

        void setup() override {};

        void routine() override {};

        void destroy() override {};
    };

    ServiceTest() = default;
};

TEST_F(ServiceTest, getId)
{
    Service1 camera{"Camera"};
    Service2 energySubsystem{"Energy"};
    EXPECT_EQ(camera.m_serviceId, "Camera") << "Problem during StaticService Id access attempt";
    EXPECT_EQ(energySubsystem.m_serviceId, "Energy") << "Problem during RoutineService Id access attempt";
}
