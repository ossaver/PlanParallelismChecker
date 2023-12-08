#include "planChecker.h"

// Constructor
PlanChecker::PlanChecker(Plan* plan)
{
	this->plan = plan;
	task = plan->task;
	this->initialState = nullptr;
}

// Generates the problem initial state
void PlanChecker::createInitialState()
{
	this->initialState = new State(0);
	int numVars = task->variables.size();
	for (int v = 0; v < numVars; v++) {
		GroundedVar& var = task->variables[v];
		bool inState = false;
		for (GroundedValue& value : var.initialValues) {
			if (value.value == task->task->CONSTANT_TRUE) {
				inState = true;
				break;
			}
		}
		if (inState) {
			initialState->add(v);
		}
	}
}

// Explores a new branch where an action of the plan can be executed before it was planned
void PlanChecker::newBranch(int planAction, int stateNumber, std::vector<State> states) {
	State* baseState = stateNumber >= 0 ? &states[stateNumber] : initialState;
	int time = baseState->time;
	int newStateTime = time + 1;
	if (newStateTime >= bestMakespan)
		return;
	PlanAction& pa = plan->actions[planAction];
	State newState(newStateTime);
	newState.generate(baseState, &pa, planAction, task->task);
	while (stateNumber < states.size()) {
		if (stateNumber == states.size() - 1) break;
		if (states[stateNumber + 1].time <= newStateTime)
			stateNumber++;
		else break;
	}
	if (stateNumber < 0) 
		states.insert(states.begin(), newState);
	else 
		states.insert(states.begin() + stateNumber + 1, newState);
	State* prevState = &newState;
	for (int i = stateNumber + 2; i < states.size(); i++) {
		states[i].generate(prevState, &plan->actions[states[i].planAction], states[i].planAction, task->task);
		prevState = &states[i];
	}
	search(planAction + 1, &states);
}

// Searches for the best action ordering to minimize makespan
void PlanChecker::search(int planAction, std::vector<State>* states)
{
	std::vector<int> validStates;
	while (planAction < plan->actions.size()) {
		PlanAction& pa = plan->actions[planAction];
		validStates.clear();
		if (executable(pa.a, initialState)) {
			validStates.push_back(-1);
		}
		for (int i = 0; i < states->size(); i++) {
			State* s_current = &states->at(i);
			if (i == states->size() - 1 || s_current->time < states->at(i + 1).time) {
				if (executable(pa.a, s_current)) {
					validStates.push_back(i);
				}
			}
		}
		if (validStates.empty()) return;
		int inCurrentTime = -2;
		for (int i : validStates) {
			int time = i < 0 ? 0 : states->at(i).time;
			if (time <= pa.time) { // No nos interesa si se puede retrasar, solo adelantar
				if (time < pa.time) { // Se puede adelantar
					if (!conflicts(time, &pa, states)) {
						newBranch(planAction, i, *states);
					}
				}
				else if (time == pa.time && inCurrentTime == -2) {
					inCurrentTime = i;
				}
			}
		}
		if (inCurrentTime != -2) { // Generate new state at the end and continue
			int newStateTime = pa.time + 1;
			if (newStateTime >= bestMakespan)
				return;
			State* baseState = states->empty() ? initialState : &states->back();
			State newState(newStateTime);
			newState.generate(baseState, &pa, planAction, task->task);
			states->push_back(newState);
			planAction++;
		}
		else return;
	}
	int makespan = 0;
	for (State& s : *states)
		if (s.time > makespan)
			makespan = s.time;
	if (makespan < bestMakespan) {
		bestMakespan = makespan;
		bestPlan = *states;
	}
}

// Checks if an action is executable in a state
bool PlanChecker::executable(GroundedAction* a, State* s)
{
	for (GroundedCondition& c : a->startCond) {
		if (c.valueIndex == task->task->CONSTANT_TRUE) {
			if (!s->hold(c.varIndex)) {
				return false;
			}
		}
		else {
			if (s->hold(c.varIndex))
				return true;
		}
	}
	return true;
}

// Prints a plan
void PlanChecker::printPlan(std::vector<State>* states) {
	for (State& s : *states) {
		cout << (s.time - 1) << ": " << plan->actions[s.planAction].a->getName(task->task)
			<< " [" << s.offset << "]" << endl;
	}
}

// Starts the search
void PlanChecker::start()
{
	createInitialState();
	std::vector<State> states;
	search(0, &states);
	cout << "Makespan: " << bestMakespan << " [" << plan->makespan() << "]" << endl;
	printPlan(&bestPlan);
}

// Destructor
PlanChecker::~PlanChecker()
{
	if (initialState != nullptr)
		delete initialState;
}

// Checks if two actions can be extecuted simultaneously
bool PlanChecker::simultaneous(GroundedAction* a1, GroundedAction* a2) {
	for (GroundedCondition& c1 : a1->startEff) {
		for (GroundedCondition& c2 : a2->startEff) {
			if (c1.varIndex == c2.varIndex && c1.valueIndex != c2.valueIndex)
				return false;
		}
	}
	for (GroundedCondition& c1 : a1->endEff) {
		for (GroundedCondition& c2 : a2->endEff) {
			if (c1.varIndex == c2.varIndex && c1.valueIndex != c2.valueIndex)
				return false;
		}
	}
	for (GroundedCondition& c1 : a1->startCond) {
		for (GroundedCondition& c2 : a2->startEff) {
			if (c1.varIndex == c2.varIndex && c1.valueIndex != c2.valueIndex)
				return false;
		}
	}
	for (GroundedCondition& c1 : a1->endCond) {
		for (GroundedCondition& c2 : a2->endEff) {
			if (c1.varIndex == c2.varIndex && c1.valueIndex != c2.valueIndex)
				return false;
		}
	}
	for (GroundedCondition& c1 : a2->startCond) {
		for (GroundedCondition& c2 : a1->startEff) {
			if (c1.varIndex == c2.varIndex && c1.valueIndex != c2.valueIndex)
				return false;
		}
	}
	for (GroundedCondition& c1 : a2->endCond) {
		for (GroundedCondition& c2 : a1->endEff) {
			if (c1.varIndex == c2.varIndex && c1.valueIndex != c2.valueIndex)
				return false;
		}
	}
	return true;
}

// Check if a condition is produced between timesteps t1 and t2
bool PlanChecker::restored(GroundedCondition& c, int t1, int t2, std::vector<State>* states) {
	for (State& s : *states) {
		int actionTime = s.time - 1;
		if (actionTime >= t1 && actionTime <= t2) {
			for (GroundedCondition& e : plan->actions[s.planAction].a->startEff) {
				if (e.varIndex == c.varIndex && e.valueIndex == c.valueIndex)
					return true;
			}
			for (GroundedCondition& e : plan->actions[s.planAction].a->endEff) {
				if (e.varIndex == c.varIndex && e.valueIndex == c.valueIndex)
					return true;
			}
		}
	}
	return false;
}

// Checks if an action produces conflicts with later states
bool PlanChecker::conflicts(int time, PlanAction* pa, std::vector<State>* states)
{
	// Try to put action at time "time". Check simultaneous actions and later ones
	for (State& s : *states) {
		int actionTime = s.time - 1;
		if (actionTime == time) { // Simultaneous
			if (!simultaneous(pa->a, plan->actions[s.planAction].a))
				return true;
		}
		else if (actionTime > time) { // Later
			for (GroundedCondition& e : pa->a->startEff) {
				for (GroundedCondition& c : plan->actions[s.planAction].a->startCond) {
					if (e.varIndex == c.varIndex && e.valueIndex != c.valueIndex) { // Deletes a precondition of later action
						if (!restored(c, time + 1, actionTime, states))
							return true;
					}
				}
			}
			for (GroundedCondition& e : pa->a->endEff) {
				for (GroundedCondition& c : plan->actions[s.planAction].a->endCond) {
					if (e.varIndex == c.varIndex && e.valueIndex != c.valueIndex) { // Deletes a precondition of later action
						if (!restored(c, time + 1, actionTime, states))
							return true;
					}
				}
			}
		}
	}
	return false;
}

// Checks if a variable holds in a state
bool State::hold(TVariable v)
{
	for (TVariable w : state) {
		if (w == v)
			return true;
	}
	return false;
}

// Adds a variable to a state
void State::add(TVariable v)
{
	if (!hold(v))
		state.push_back(v);
}

// Deletes a variable from a state
void State::del(TVariable v)
{
	for (int i = 0; i < state.size(); i++) {
		if (state[i] == v) {
			state.erase(state.begin() + i);
			return;
		}
	}
}

// Generates a new state by applying the given action to a state
void State::generate(State* s, PlanAction* pa, int planAction, ParsedTask* task)
{
	this->state.clear();
	this->planAction = planAction;
	this->offset = this->time - 1 - pa->time;
	for (int v : s->state) this->state.push_back(v);
	for (GroundedCondition& c : pa->a->startEff) {
		if (c.valueIndex == task->CONSTANT_FALSE)
			del(c.varIndex);
	}
	for (GroundedCondition& c : pa->a->startEff) {
		if (c.valueIndex == task->CONSTANT_TRUE)
			add(c.varIndex);
	}
	for (GroundedCondition& c : pa->a->endEff) {
		if (c.valueIndex == task->CONSTANT_FALSE)
			del(c.varIndex);
	}
	for (GroundedCondition& c : pa->a->endEff) {
		if (c.valueIndex == task->CONSTANT_TRUE)
			add(c.varIndex);
	}
}

// Prints a plan
void State::print(GroundedTask* task)
{
	for (TVariable v : state) {
		cout << task->variables[v].toString(task->task) << endl;
	}
}
