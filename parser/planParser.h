#ifndef PLAN_PARSER_H
#define PLAN_PARSER_H

#include "syntaxAnalyzer.h"
#include "plan.h"
#include "../grounder/groundedTask.h"

/********************************************************/
/* Oscar Sapena Vercher - DSIC - UPV                    */
/* December 2023                                        */
/********************************************************/
/* Parses a plan file.                                  */
/********************************************************/

class PlanParser {
private:
    SyntaxAnalyzer* syn;
    GroundedTask* task;

    void parseAction(std::string name, Plan* plan);

public:
    PlanParser();
    ~PlanParser();
    Plan* parsePlan(char* planFileName, GroundedTask* task);
};

#endif
