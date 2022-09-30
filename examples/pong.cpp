#include <frameworkd/classes/daemon/daemon.hpp>
#include <iostream>

class PongService : public StaticService {
    int pongCounter = 0;

public:
    PongService(std::string serviceId)
        : StaticService { serviceId }
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
        DBusHandler::registerSignal(pongSignalPath);

        DBusHandler::registerMethod(pongMethodPath, [&](nlohmann::json pingWrapper) {
            pongCounter = pingWrapper["ping"];
            pongCounter++;
            pingWrapper["ping"] = pongCounter;

            std::cout << "Pong: " << pongCounter << std::endl;

            DBusHandler::emitSignal(pongSignalPath, pingWrapper);

            return pingWrapper;
        });
    }

    void destroy() override
    {
    }
};

int main()
{
    Daemon newDaemon("pong");
    PongService pongService("pong");
    newDaemon.deploy(pongService);
    newDaemon.run();
    return 0;
}
