#include "../src/classes/daemon/daemon.hpp"
#include <iostream>

class PingService : public RoutineService {
    int pingCounter = 0;
    std::chrono::nanoseconds pongDelay;

public:
    PingService(std::string serviceId)
        : RoutineService { serviceId }
    {
    }

    const DBusHandler::Path pongMethodPath {
        "zfkd.dbus.pong",
        "/zfkd/dbus/pong",
        "zfkd.dbus.pong",
        "pong"
    };

    const DBusHandler::Path pongSignalPath {
        "zfkd.dbus.pong",
        "/zfkd/dbus/pong",
        "zfkd.dbus.pong",
        "pongDelay"
    };

    void setup() override
    {

        DBusHandler::subscribeToSignal(pongSignalPath, [&](nlohmann::json pongWrapper) {
            std::chrono::nanoseconds finalTime;
            finalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

            pongDelay = finalTime - pongDelay;
            std::cout << "Delay: " << pongDelay.count() << "ns" << std::endl;
        });

    }

    void routine() override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2000 + 200));
        pongDelay = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

        nlohmann::json pingWrapper;
        pingWrapper["ping"] = pingCounter;

        nlohmann::json pongWrapper = DBusHandler::callMethod(pongMethodPath, pingWrapper);
        pingCounter = pongWrapper["ping"];

        pingCounter++;

        std::cout << "Ping: " << pingCounter << std::endl;
    }

    void destroy() override
    {
    }
};

int main()
{
    Daemon newDaemon("PING.json");
    PingService pingService("PING");
    newDaemon.deploy(pingService);
    newDaemon.run();
    return 0;
}