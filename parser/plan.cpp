#include "plan.h"

Plan::Plan(GroundedTask* task)
{
	this->task = task;
	time = 0;
}

Plan::~Plan()
{
}

void Plan::newTimestep()
{
	if (!actions.empty() && actions.back().time == time)
		time++;
}

bool Plan::addAction(std::string name, std::vector<std::string>& params)
{
	bool found = false;
	for (GroundedAction& a : task->actions) {
		if (compareStr(a.name, name)) {
			if (a.parameters.size() == params.size()) {
				found = true;
				for (int i = 0; i < params.size(); i++) {
					int p = a.parameters[i];
					if (!compareStr(params[i], task->task->objects[p].name)) {
						found = false;
						break;
					}
				}
				if (found) {
					this->actions.emplace_back(this->time, &a);
					break;
				}
			}
		}
	}
	return found;
}

int Plan::makespan()
{
	int ms = 0;
	for (PlanAction& a : actions) {
		if (a.time + 1 > ms)
			ms = a.time + 1;
	}
	return ms;
}

PlanAction::PlanAction(int time, GroundedAction* a)
{
	this->time = time;
	this->a = a;
}
