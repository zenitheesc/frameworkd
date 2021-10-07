
#pragma once
#include <mutex>
#include <string>

class IService {
public:
    std::string m_serviceId;

protected:
    //	DBusHandler::DBusHandler m_dbus;

public:
    virtual void setup() = 0;
    virtual void routine() = 0;
    virtual void destroy() = 0;
    explicit IService(std::string serviceId)
        : m_serviceId { serviceId }
    {
    }

	virtual ~IService() = default;
};

