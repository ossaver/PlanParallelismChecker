#include "planParser.h"
#include "../utils/utils.h"
#include <iostream>
using namespace std;

// Constructor
PlanParser::PlanParser() {
	task = nullptr;
	syn = nullptr;
}

// Destructor
PlanParser::~PlanParser() {

}

// Parses a plan file
Plan* PlanParser::parsePlan(char* planFileName, GroundedTask* task) {
	this->task = task;
	this->syn = new SyntaxAnalyzer(planFileName);
	bool endParsing = false;
	Plan* plan = new Plan(task);
	while (!endParsing) {
		string name = syn->readName();
		if (name.compare("done") == 0) {
			endParsing = true;
		}
		else if (name.compare("timestep") == 0) {
			plan->newTimestep();
		}
		else {
			parseAction(name, plan);
		}
	}
	delete syn;
	return plan;
}

// Parses a plan action
void PlanParser::parseAction(std::string name, Plan* plan)
{
	syn->openPar();
	std::vector<string> params;
	Token* token = syn->readSymbol(2, Symbol::CLOSE_PAR, Symbol::NAME);
	while (!syn->isSym(token, Symbol::CLOSE_PAR)) {
		params.push_back(token->description);
		syn->readColon();
		syn->readName();	// Type
		token = syn->readSymbol(2, Symbol::CLOSE_PAR, Symbol::NAME);
		if (!syn->isSym(token, Symbol::CLOSE_PAR)) {
			if (token->description.compare(",") == 0) {
				token->description = syn->readName();
			}
			else if (token->description.starts_with(",")) {
				token->description = token->description.substr(1);
			}
			else syn->notifyError("A comma was expected");
		}
	}
	if (!plan->addAction(name, params))
		syn->notifyError("Action not grounded");
}
