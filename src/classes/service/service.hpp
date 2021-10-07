
#pragma once
#include <mutex>
#include <string>

class IService {
public:
    /* Id of the service */
	std::string m_serviceId;

protected:
    //	DBusHandler::DBusHandler m_dbus;

public:
    
	/**
	 * @brief	An interface function to be implemented
	 * 			by the user to make the "setup" of its 
	 * 			class' parameter members.
	 */
	virtual void setup() = 0;
    
	/**
	 * @brief	An interface function to be implemented
	 * 			by the user to make the "setup" of its 
	 * 			class' parameter members.
	 */
	virtual void routine() = 0;
    
	/**
	 * @brief	An interface function to be implemented
	 * 			by the user to make the "setup" of its 
	 * 			class' parameter members.
	 */
	virtual void destroy() = 0;
    
	/**
	 * @brief	An interface function to be implemented
	 * 			by the user to make the "setup" of its 
	 * 			class' parameter members.
	 */
	explicit IService(std::string serviceId)
        : m_serviceId { serviceId }
    {
    }

	/**
	 * @brief	A virtual destructor to ensure 
	 * 			inheriance compatibility.
	 */
	virtual ~IService() = default;
};

