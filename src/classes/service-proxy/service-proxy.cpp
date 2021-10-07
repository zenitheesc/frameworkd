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
	while(status.state == Status::STOPED){
		status.mtx.unlock();
		routine.loop();
		status.mtx.lock();
	}
	status.mtx.unlock();
}

RoutineHandler::RoutineHandler(IRoutine& routine, nlohmann::json& configs) {
	(*this->routine) = routine;
	this->status.mtx.lock();
	this->status.state = Status::MISSINGDEPENDENCIES;
	this->status.mtx.unlock();
	this->depsRefState = configs["dependencies"];
	this->depsCrntState = configs["dependencies"];
	for(nlohmann::json& dependencie : this->depsCrntState) {
		dependencie["status"] = Status::MISSINGDEPENDENCIES;
	}
	this->info.data = configs;
	
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
	std::lock_guard<std::mutex> lck(info.occupied);
	status.mtx.lock();
	info.data["state"] = status.state;
	status.mtx.unlock();
	return info.data;

}

nlohmann::json RoutineHandler::update(void) {
	int missingDeps = depsRefState["lenght"];
	for(int i = 0; i < depsRefState["lenght"]; i++) {
		using namespace nlohmann;
		const json& dependencie = depsCrntState[i];
		const json& dependencieRef = depsRefState[i];
		if(dependencie["status"] == dependencieRef["status"]) {
			missingDeps--;
		}
	}
	if(!missingDeps) {
		run();
		nlohmann::json changes = getStatus();
		return (nlohmann::json) {{{"changed", true}}, changes};
	}
	status.mtx.lock();
	if(status.state == Status::RUNNING) {
		status.mtx.unlock();
		stop();
		status.mtx.lock();
		status.state = Status::MISSINGDEPENDENCIES;
		status.mtx.unlock();
		nlohmann::json changes = getStatus();
		return (nlohmann::json) {{{"changed", true}}, changes};
	}
	status.mtx.unlock();
	return (nlohmann::json) {{"changed", false}};
}

