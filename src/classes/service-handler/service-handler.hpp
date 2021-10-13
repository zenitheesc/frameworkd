
#pragma once
#include <thread>
#include <mutex>
#include "../service-proxy/service-proxy.hpp"
#include "../service/service.hpp"

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

