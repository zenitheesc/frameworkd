#include "../src/classes/dbus-handler/dbus-handler.hpp"

#include <chrono>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class ServerClient : public ::testing::Test {
public:
    DBusHandler m_client {};

    static DBusHandler::Path m_methodPath;
    static DBusHandler::Path m_signalPath;
    static DBusHandler::Path m_propertiesPath;
    static bool isServerConfig;
    static nlohmann::json m_properties;

    static void SetUpTestSuite()
    {
        isServerConfig = true;

        m_properties["num"] = 9;

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
        server.exposeProperty(
            m_propertiesPath, [&]() { return m_properties; }, [&](nlohmann::json req) { m_properties = req; });

        isServerConfig = false;
        server.finish();
    }
};

DBusHandler::Path ServerClient::m_methodPath {
    "org.sdbuscpp.concatenator",
    "/org/sdbuscpp/concatenator",
    "org.sdbuscpp.Concatenator",
    "concatenate"
};

DBusHandler::Path ServerClient::m_signalPath {
    "org.sdbuscpp.concatenator",
    "/org/sdbuscpp/concatenator",
    "org.sdbuscpp.Concatenator",
    "concatenated"
};

DBusHandler::Path ServerClient::m_propertiesPath {
    "org.sdbuscpp.concatenator",
    "/org/sdbuscpp/concatenator",
    "org.sdbuscpp.Concatenator",
    "properties"
};

bool ServerClient::isServerConfig;
nlohmann::json ServerClient::m_properties;

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
    int num = 4;

    arg["num"] = num;

    m_client.finish();
    m_client.callMethodAsync(m_methodPath, arg, [&](nlohmann::json res) {
        EXPECT_EQ(num * 2, res["num"]);
    });
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

TEST_F(ServerClient, Get)
{
    m_client.finish();

    nlohmann::json res = m_client.getProperty(m_propertiesPath);
    EXPECT_EQ(res, m_properties);
}

TEST_F(ServerClient, AsyncGet)
{
    m_client.finish();

    m_client.getProperty(m_propertiesPath, [&](nlohmann::json arg) {
        EXPECT_EQ(arg, m_properties);
    });
}

TEST_F(ServerClient, Set)
{
    nlohmann::json arg;
    arg["num"] = static_cast<int>(m_properties["num"]) * 4;

    m_client.finish();
    m_client.setProperty(m_propertiesPath, arg);

    EXPECT_EQ(arg, m_properties);
}
