#include "../src/classes/dbus-handler/dbus-handler.hpp"
#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <thread>

class ServerClient : public ::testing::Test {
protected:
    PathHandler::DBusPath m_serverPath = {
        "org.sdbuscpp.concatenator",
        "/org/sdbuscpp/concatenator",
        "org.sdbuscpp.Concatenator",
        "concatenate"
    };
    DBusHandler m_client;

    void SetUp() override
    {
        std::thread t1(&ServerClient::server, this);
        t1.detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void server()
    {
        DBusHandler server(m_serverPath.service);
        server.registerMethod(m_serverPath, [](nlohmann::json req) {
            nlohmann::json res;
            int num = req["num"];
            res["num"] = num * 2;
            return res;
        });

        server.finish();
    }
};

TEST_F(ServerClient, Method)
{
    nlohmann::json arg;
    nlohmann::json res;

    int num = 4;
    arg["num"] = num;
    res = m_client.callMethod(m_serverPath, arg);
    EXPECT_EQ(num * 3, res["num"]);
}
