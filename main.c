#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "inputParser.h"
#include "schedulerUtils.h"
#include "edfScheduler.h"
#include "resourceManager.h"

int main(int argc, char* argv[]) {
    Task tasks[100];
    int taskCount = 0;
    int resourceCount = 0;

   
    parseInput("input.txt", tasks, &taskCount, &resourceCount);

    
    printf("Tasks Loaded: %d\n", taskCount);
    printf("Resources Available: %d\n", resourceCount);
    


    if (!isSchedulable(tasks, taskCount)) {
        printf("\n WARNING: Task set may not be schedulable!\n");
        return 0;
    } else {
        printf("\n Task set is schedulable under EDF + PIP.\n\n");
    }

    printf("Running with Priority Inheritance Protocol (PIP)...\n");
    runPIPScheduler(tasks, taskCount, resourceCount);


    printf("Running with Priority Ceiling Protocol (PCP)...\n");
    runPCPScheduler(tasks, taskCount, resourceCount);

    return 0;
}