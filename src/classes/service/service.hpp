/*
* routine.hpp
* 
* Author: Carlos Craveiro (@CarlosCraveiro)
* Created On: September 11, 2021
* 
*/

#pragma once
#include <string>
#include <mutex>

class Status {
	public:
		enum stateT { 
			MISSINGDEPENDENCIES = 0, UNINITIALIZED, INITIALIZED, RUNNING, STOPED, DEAD

		};
		stateT state;
		std::mutex mtx;
};

class IRoutine {
	public:
		std::string id;
	public:
		virtual void onLoad()=0;
		virtual void loop()=0;
		virtual void unload()=0;
		IRoutine(std::string id): id{id} {}
		~IRoutine(void){};
};

