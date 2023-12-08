#include <stdexcept>
#include "utils/utils.h"
#include "parser/parsedTask.h"
#include "preprocess/preprocess.h"
#include "grounder/grounder.h"
#include "parser/parser.h"
#include "parser/planParser.h"
#include "checker/planChecker.h"

/*********************************************************/
/* Oscar Sapena Vercher - DSIC - UPV                     */
/* November 2023                                         */
/*********************************************************/
/* Checks the plan parallelism                           */
/*********************************************************/

// Command-line parameters
struct PlannerParameters {
    char* domainFileName;
    char* problemFileName;
    char* planFileName;

    PlannerParameters() : domainFileName(nullptr), problemFileName(nullptr), planFileName(nullptr) { }
};

// Prints the command-line arguments of the planner
void printUsage() {
    cout << "Usage: checkTimeSteps <domain_file> <problem_file> <plan_file>" << endl;
}

// Parses the domain and problem files
ParsedTask* parseStage(PlannerParameters* parameters) {
    Parser parser;
    ParsedTask* parsedTask = parser.parseDomain(parameters->domainFileName);
    parser.parseProblem(parameters->problemFileName);
    return parsedTask;
}

// Preprocesses the parsed task
PreprocessedTask* preprocessStage(ParsedTask* parsedTask, PlannerParameters* parameters) {
    Preprocess preprocess;
    PreprocessedTask* prepTask = preprocess.preprocessTask(parsedTask);
    return prepTask;
}

// Grounder stage of the preprocessed task
GroundedTask* groundingStage(PreprocessedTask* prepTask, PlannerParameters* parameters) {
    Grounder grounder;
    GroundedTask* gTask = grounder.groundTask(prepTask, false);
    return gTask;
}

// Checks the plan
void checkPlan(char* planFileName, GroundedTask* task) {
        PlanParser planParser;
        Plan* plan = planParser.parsePlan(planFileName, task);
        if (plan != nullptr) {
            PlanChecker checker(plan);
            checker.start();
        }
}

// Sequential calls to the main planning stages
void startChecking(PlannerParameters* parameters) {
    try {
        ParsedTask* parsedTask = parseStage(parameters);
        if (parsedTask != nullptr) {
            PreprocessedTask* prepTask = preprocessStage(parsedTask, parameters);
            if (prepTask != nullptr) {
                GroundedTask* gTask = groundingStage(prepTask, parameters);
                if (gTask != nullptr) {
                    checkPlan(parameters->planFileName, gTask);
                    delete gTask;
                }
                delete prepTask;
            }
            delete parsedTask;
        }
    }
    catch (std::invalid_argument& e)
    {
        cerr << e.what() << endl;
    }
    catch (PlannerException& e)
    {
        cerr << e.what() << endl;
    }
}

// Main method
int main(int argc, char* argv[]) {
    PlannerParameters parameters;
    if (argc < 4) {
        printUsage();
    }
    else {
        bool error = false;
        int param = 1;
        while (param < argc && !error) {
            if (argv[param][0] != '-') {
                if (parameters.domainFileName == nullptr)
                    parameters.domainFileName = argv[param];
                else if (parameters.problemFileName == nullptr)
                    parameters.problemFileName = argv[param];
                else if (parameters.planFileName == nullptr)
                    parameters.planFileName = argv[param];
                else error = true;
            } else  error = true;
            param++;
        }
        if (error || parameters.domainFileName == nullptr || parameters.problemFileName == nullptr || parameters.planFileName == nullptr) {
            printUsage();
        }
        else {
            startChecking(&parameters);
        }
    }

    return 0;
}
