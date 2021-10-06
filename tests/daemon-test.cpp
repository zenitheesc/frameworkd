#include "../src/classes/dbus-handler/dbus-handler.hpp"
#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <thread>

PathHandler::DBusPath m_methodPath = {
    "org.sdbuscpp.concatenator",
    "/org/sdbuscpp/concatenator",
    "org.sdbuscpp.Concatenator",
    "concatenate"
};
PathHandler::DBusPath m_signalPath = {
    "org.sdbuscpp.concatenator",
    "/org/sdbuscpp/concatenator",
    "org.sdbuscpp.Concatenator",
    "concatenated"
};

bool isServerConfig;

class ServerClient : public ::testing::Test {
public:
    DBusHandler m_client;

    static void SetUpTestSuite()
    {
        isServerConfig = true;
        std::thread t1(&ServerClient::server);
        t1.detach();
        while (isServerConfig) { };
    }

    static void server()
    {
        DBusHandler server(m_methodPath.service);
        server.registerSignal(m_signalPath);
        server.registerMethod(m_methodPath, [&](nlohmann::json req) {
            nlohmann::json res;
            int num = req["num"];
            res["num"] = num * 2;
            server.emitSignal(m_signalPath, res);
            return res;
        });

        isServerConfig = false;
        server.finish();
    }
};

TEST_F(ServerClient, Method)
{
    nlohmann::json arg;
    nlohmann::json res;

    int num = 4;
    arg["num"] = num;
    res = m_client.callMethod(m_methodPath, arg);

    EXPECT_EQ(num * 2, res["num"]);
}

TEST_F(ServerClient, AsyncMethod)
{
    nlohmann::json arg;
    bool isCalled = false;
    int num = 4;

    arg["num"] = num;

    m_client.finish();
    m_client.callMethodAsync(m_methodPath, arg, [&](nlohmann::json res) {
        EXPECT_EQ(num * 2, res["num"]);
        isCalled = true;
    });
    while (!isCalled) { }
}

TEST_F(ServerClient, Signal)
{
    nlohmann::json arg;
    nlohmann::json res;
    int num = 4;
    m_client.subscribeToSignal(m_signalPath, [&](nlohmann::json rec) {
        EXPECT_EQ(num * 2, rec["num"]);
    });
    m_client.finish();
    arg["num"] = num;

    res = m_client.callMethod(m_methodPath, arg);
}

