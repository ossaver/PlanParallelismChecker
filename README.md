# PlanParallelismChecker
Checks the parallelism of the actions in a plan.

The program can be compiled with the following command:

```
make
```

The program runs with the following parameters:
```
./parallelismChecker domain.pddl problem.pddl plan
```

The domain and problem must be strips PDDL files. The plan must have the following format:

```
action1
...
actionN
timestep
action1
...
actionN
timestep
...
```
Where the *timestep* keyword separates blocks of actions that can be executed in parallel. The actions must have the following format:

```
operator-name(parameter1:type,parameter2:type,...)
```

If no types have been defined in the domain, the *default* keyword can be used.
