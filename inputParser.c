#include <stdio.h>
#include <stdlib.h>
#include "inputParser.h"

void parseInput(char* filename, Task tasks[], int* taskCount, int* resourceCount){

    FILE* fp = fopen(filename, "r");

    if(fp == NULL){
        printf("❌ Error opening file\n");
        exit(1);
    }

    fscanf(fp, "%d", resourceCount);
    fscanf(fp, "%d", taskCount);

    for(int i = 0; i < *taskCount; i++){

        Task* t = &tasks[i];

        fscanf(fp, "%d %d %d %d %d",
               &t->taskId,
               &t->executionTime,
               &t->period,
               &t->deadline,
               &t->resourceCount);

        for(int j = 0; j < t->resourceCount; j++){

            fscanf(fp, "%d %d %d",
                   &t->resources[j].resourceId,
                   &t->resources[j].startTime,
                   &t->resources[j].duration);
        }
    }

    fclose(fp);
}