#include <stdio.h>
#include <stdlib.h>
#include "inputParser.h"

int validate(Task t) {
    if (t.executionTime <= 0 || t.period <= 0 || t.deadline <= 0)
        return 0;
    return 1;
}

int readInput(const char* fileName, Task tasks[], int* resourceCount) {

    FILE* fp = fopen(fileName, "r");
    if (!fp) {
        printf("File error\n");
        exit(1);
    }

    int taskCount;

    fscanf(fp, "%d", resourceCount);
    fscanf(fp, "%d", &taskCount);

    for (int i = 0; i < taskCount; i++) {

        fscanf(fp, "%d %d %d %d %d",
            &tasks[i].taskId,
            &tasks[i].period,
            &tasks[i].executionTime,
            &tasks[i].deadline,
            &tasks[i].resourceCount);

        if (!validate(tasks[i])) {
            printf("Invalid task parameters\n");
            exit(1);
        }

        for (int j = 0; j < tasks[i].resourceCount; j++) {
            fscanf(fp, "%d %d %d",
                &tasks[i].resources[j].resourceId,
                &tasks[i].resources[j].startTime,
                &tasks[i].resources[j].duration);
        }
    }

    fclose(fp);
    return taskCount;
}