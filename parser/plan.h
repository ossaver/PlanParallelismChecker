#ifndef _PLAN
#define _PLAN
#include "../grounder/groundedTask.h"

class PlanAction {
public:
    GroundedAction* a;
    int time;

    PlanAction(int time, GroundedAction* a);
};

class Plan {
private:
    int time;

public:
    std::vector<PlanAction> actions;
    GroundedTask* task;

    Plan(GroundedTask* task);
    ~Plan();
    void newTimestep();
    bool addAction(std::string name, std::vector<std::string>& params);
    int makespan();
};

#endif // !_PLAN
