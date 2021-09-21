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
			UNKNOWN = 0, MISSINGDEPENDENCIES, UNINITIALIZED, INITIALIZED, RUNNING, STOPPED, DEAD
		};

	private:
		stateT state;
		std::mutex mtx;

	public:
		stateT getState(void);
		void setState(stateT newState);
};

class IRoutine {
	public:
		std::string id;
	public:
		virtual void onLoad()=0;
		virtual void loop()=0;
		virtual void unLoad()=0;
		IRoutine(std::string id): id{id} {}
		~IRoutine(void){};
};

