#ifndef _PLAN_CHECKER
#define _PLAN_CHECKER

#include "../parser/plan.h"

/********************************************************/
/* Oscar Sapena Vercher - DSIC - UPV                    */
/* December 2023                                        */
/********************************************************/
/* Checks the plan parallelism.                         */
/********************************************************/

// State
class State {
public:
	int time;
	std::vector<TVariable> state;
	int planAction;
	int offset;

	State(int time) { this->time = time; this->planAction = -1; this->offset = 0; }
	bool hold(TVariable v);
	void add(TVariable v);
	void del(TVariable v);
	void generate(State* s, PlanAction* pa, int planAction, ParsedTask* task);
	void print(GroundedTask* task);
};

// Plan parallelism checker
class PlanChecker {
private:
	Plan* plan;
	GroundedTask* task;
	State* initialState;
	int bestMakespan = MAX_INT32;
	std::vector<State> bestPlan;

	inline static TVarValue getVariableValueCode(unsigned int var, unsigned int value) {
		return (var << 16) + value;
	}
	inline static TVariable getVariableIndex(TVarValue code) {
		return code >> 16;
	}
	inline static TValue getValueIndex(TVarValue code) {
		return code & 0xFFFF;
	}
	void createInitialState();
	void search(int planAction, std::vector<State>* states);
	bool executable(GroundedAction* a, State* s);
	bool conflicts(int time, PlanAction* pa, std::vector<State>* states);
	bool simultaneous(GroundedAction* a1, GroundedAction* a2);
	bool restored(GroundedCondition& c, int t1, int t2, std::vector<State>* states);
	void newBranch(int planAction, int stateNumber, std::vector<State> states);
	void printPlan(std::vector<State>* states);

public:
	PlanChecker(Plan* plan);
	~PlanChecker();
	void start();
};


#endif
