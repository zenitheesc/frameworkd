/*
* routine.hpp
* 
* Author: Carlos Craveiro (@CarlosCraveiro)
* Created On: September 11, 2021
* 
*/

#pragma once

typedef enum state_t { 
	MISSINGDEPENDENCIES = 0, UNINITIALIZED, INITIALIZED, RUNNING, STOPED, DEAD

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

