/*
* service.cpp
* 
* Author: Carlos Craveiro (@CarlosCraveiro)
* Created On: September 11, 2021
* 
*/

#include "routine.hpp"
#include <mutex>

Status::stateT Status::getState(void) {
	std::lock_guard<std::mutex> lock(mtx);
	return state;
}

void Status::setState(Status::stateT newState) {
	std::lock_guard<std::mutex> lock(mtx);
	state = newState;
}

