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

    // 1. Parse the input file
    // Assumes input.txt follows your format:
    // [numResources]
    // [numTasks]
    // [taskId] [exec] [period] [deadline] [numRes]
    // [resId] [startTime] [duration]
    parseInput("input.txt", tasks, &taskCount, &resourceCount);

    printf("========================================\n");
    printf("   Real-Time Systems: EDF with PIP\n");
    printf("========================================\n");
    printf("Tasks Loaded: %d\n", taskCount);
    printf("Resources Available: %d\n", resourceCount);
    printf("----------------------------------------\n");

    // 2. Perform Schedulability Analysis
    // This uses the U + Bi/Ti <= 1 logic we discussed
    if (!isSchedulable(tasks, taskCount)) {
        printf("\n⚠️  WARNING: Task set may not be schedulable!\n");
        printf("Proceeding with simulation anyway...\n\n");
    } else {
        printf("\n✅ Task set is schedulable under EDF + PIP.\n\n");
    }

    // 3. Run the Event-Driven Scheduler
    // We pass PROTOCOL_PIP to trigger the inheritance logic
    runEdfScheduler(tasks, taskCount, resourceCount, PROTOCOL_PIP);

    printf("\n----------------------------------------\n");
    printf("Simulation Complete.\n");
    printf("========================================\n");

    return 0;
}