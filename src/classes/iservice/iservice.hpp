
#pragma once
#include <iostream>
#include <string>
class IService {
public:
    /* Id of the service */
    std::string m_serviceId;

protected:
    //	DBusHandler::DBusHandler m_dbus;

public:
  /**
   * @brief	An interface function to be implemented by
   * 		the user to make the "setup"/configuration
   * 		of its class' parameter-members.
   */
    virtual void setup() = 0;

  /**
   * @brief An interface function to be implemented by 
   *        the user if it wants to create a 
   *        Routine Service. If it's implemented, 
   *        this function is going to run in a loop, so its 
   *        instructions are going to be executed "routinely".
   */
    virtual void routine() = 0;

  /**
   * @brief An interface function to be implemented by
   * 		the user to execute the needed instructions
   * 		to safely "destroy" the resources used by the
   * 		class' previous functions.
   */
    virtual void destroy() = 0;

  /**
   * @brief	The constructor of this interface.
   *
   * @param serviceId std::string that is the id related
   * 		to the instantiated class.
   */

    explicit IService(std::string serviceId)
        : m_serviceId { serviceId }
    {
    }

  /**
   * @brief	A virtual destructor to ensure
   * 		inheritance compatibility.
   */
    virtual ~IService() = default;
};

class StaticService : public IService {
public:
    explicit StaticService(std::string serviceId)
        : IService(serviceId)
    {
    }

    void routine() final { }

    ~StaticService() override = default;
};

class RoutineService : public IService {
public:
    explicit RoutineService(std::string serviceId)
        : IService(serviceId)
    {
    }

    ~RoutineService() override = default;
};

