/*
* routine-handler.cpp
*
* Author: Carlos Craveiro (@CarlosCraveiro)
* Created On: September 11, 2021
*/

#include "routine-handler.hpp"

void routineModule(IRoutine& routine, Status& status) {
	routine.onLoad();
	status.mtx.lock();
	status.state = Status::RUNNING;
	status.mtx.unlock();
	while(status.state == Status::STOPED){
		status.mtx.unlock();
		routine.loop();
		status.mtx.lock();
	}
	status.mtx.unlock();
}

RoutineHandler::RoutineHandler(IRoutine& routine, nlohmann::json dependecies, nlohmann::json currentDependecies) {
	(*this->routine) = routine;
	this->status.mtx.lock();
	this->status.state = Status::UNINITIALIZED;
	this->status.mtx.unlock();
	(*this->dependecies) = dependecies;
	this->currentDependecies = currentDependecies;
}

RoutineHandler::~RoutineHandler() {
	//DO NOTHING
}

void RoutineHandler::run(void) {
	std::thread thread(routineModule, std::ref(*routine), std::ref(status));
	status.mtx.lock();
	status.state = Status::INITIALIZED;
	status.mtx.unlock();
	std::swap(thread, innerThread);
}

void RoutineHandler::stop(void) {
	status.mtx.lock();
	status.state = Status::STOPED;
	status.mtx.unlock();
	innerThread.join();
	status.mtx.lock();
	status.state = Status::DEAD;
	status.mtx.unlock();	
}

nlohmann::json RoutineHandler::getStatus(void) {
	//DO NOTHING
	nlohmann::json myjson;
	myjson["pi"] = 3.14;
	return myjson;

}

nlohmann::json RoutineHandler::update(nlohmann::json& updateList) {
	//DO NOTHING
	nlohmann::json myjson;
	myjson["pi"] = 3.14;
	return myjson;
}

