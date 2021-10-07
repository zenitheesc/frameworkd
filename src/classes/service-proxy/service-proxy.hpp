/*
* routine-handler.hpp
* 
* Author: Carlos Craveiro (@CarlosCraveiro)
* Created On: September 11, 2021
*/

#pragma once
#include <thread>
#include <mutex>
#include "../routine/routine.hpp"

void routineModule(IRoutine& routine, Status& status);

class RoutineHandler {
	public:
		IRoutine* routine;
		struct Info {
			nlohmann::json data;
			std::mutex occupied;
		};
		Info info;
		Status status;
		std::thread innerThread;
		nlohmann::json depsRefState;
		nlohmann::json depsCrntState;
			
	public:
		void run(void);
		void stop(void);
		nlohmann::json getStatus(void);
		nlohmann::json update(void);
		RoutineHandler(IRoutine& routine, nlohmann::json& configs);
		~RoutineHandler(void);
};

