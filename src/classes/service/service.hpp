/*
 * service.hpp
 *
 * Author: Carlos Craveiro (@CarlosCraveiro)
 * Created On: September 15, 2021
 */

#pragma once
#include <thread>
#include <mutex>
#include "../routine-handler/routine-handler.hpp"

class Service {
	public:
		 std::vector<RoutineHandler> routines;
		 std::vector<EndpointHandler> endpoints;
	public:
		 nlohmann::json getServiceStatus(void);
		nlohmann::json updateAll(nlohmann::json updateList);
		nlohmann::json registerEndpoints(void);
		nlohmann::json registerRoutines(void);

		void buildEndpoint(/*IEndpoint& newEndpoint*/);
		void buildRoutine(IRoutine& newRoutine);
};

