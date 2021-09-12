/*
* routine-handler.hpp
* 
* Author: Carlos Craveiro (@CarlosCraveiro)
* Created On: September 11, 2021
*/

#pragma once
#include <thread>
#include "../routine/routine.hpp"

void routineModule(IRoutine& routine, Status& status);

class RoutineHandler {
	public:
		IRoutine* routine;
		Status status;
		std::thread innerThread;
		nlohmann::json* dependecies;
		nlohmann::json currentDependecies;
			
	public:
		void run(void);
		void stop(void);
		nlohmann::json getStatus(void);
		nlohmann::json update(nlohmann::json& updateList);
		RoutineHandler(IRoutine& routine, nlohmann::json dependecies, nlohmann::json currentDependecies);
		~RoutineHandler(void);
};

