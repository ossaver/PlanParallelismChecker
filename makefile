CC = g++
CFLAGS = -c -Wall -std=c++20 -O3
OBJS = parallelismChecker.o planChecker.o parser.o syntaxAnalyzer.o parsedTask.o preprocess.o preprocessedTask.o grounder.o groundedTask.o utils.o plan.o planParser.o

all: $(OBJS)
	$(CC) $(OBJS) -o parallelismChecker
	
parallelismChecker.o:
	$(CC) $(CFLAGS) parallelismChecker.cpp

planChecker.o:
	$(CC) $(CFLAGS) checker/planChecker.cpp

parser.o:
	$(CC) $(CFLAGS) parser/parser.cpp

planParser.o:
	$(CC) $(CFLAGS) parser/planParser.cpp

plan.o:
	$(CC) $(CFLAGS) parser/plan.cpp

syntaxAnalyzer.o:
	$(CC) $(CFLAGS) parser/syntaxAnalyzer.cpp

parsedTask.o:
	$(CC) $(CFLAGS) parser/parsedTask.cpp
	
preprocess.o:
	$(CC) $(CFLAGS) preprocess/preprocess.cpp
	
preprocessedTask.o:
	$(CC) $(CFLAGS) preprocess/preprocessedTask.cpp

grounder.o:
	$(CC) $(CFLAGS) grounder/grounder.cpp

groundedTask.o:
	$(CC) $(CFLAGS) grounder/groundedTask.cpp

utils.o:
	$(CC) $(CFLAGS) utils/utils.cpp

clean:
	rm -f *.o
	rm -f parallelismChecker

