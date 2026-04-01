#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "edfScheduler.h"
#include "priorityQueue.h"
#include "resourceManager.h"
#include "task.h"

int get_gcd(int a, int b) {
    while (b) { a %= b; int t = a; a = b; b = t; }
    return a;
}

int get_lcm(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return (a * b) / get_gcd(a, b);
}

Job* createJobInstance(Task* t, int arrivalTime, int jobNumber) {
    Job* j = (Job*)malloc(sizeof(Job));
    j->taskId = t->taskId;
    j->jobNumber = jobNumber;
    j->remainingTime = t->executionTime;
    j->executedTime = 0;
    j->absoluteDeadline = arrivalTime + t->deadline;
    j->currentPriority = j->absoluteDeadline; 
    j->originalPriority = j->absoluteDeadline;
    j->resourceIndex = 0;
    j->isBlocked = 0;
    j->holdingResourceId = -1;
    j->waitingResourceId = -1;
    j->inheritedFromTask = -1;
    return j;
}

// 1. PIP SCHEDULER

void runPIPScheduler(Task tasks[], int taskCount, int resourceCount) {
    int time = 0;
    int hyperperiod = tasks[0].period;
    for (int i = 1; i < taskCount; i++) hyperperiod = get_lcm(hyperperiod, tasks[i].period);

    PriorityQueue pq; 
    initQueue(&pq);
    Resource resources[10];
    for (int i = 0; i < resourceCount; i++) initResource(&resources[i], i + 1);

    Job* currentJob = NULL;
    int jobCount[10] = {0};

    printf("%-10s %-8s %-10s %-15s\n", "Interval", "Job", "Resource", "Inherited From");
    printf("----------------------------------------------------------\n");

    while (time < hyperperiod) {
        for (int i = 0; i < taskCount; i++) {
            if (time % tasks[i].period == 0) 
                insertJob(&pq, createJobInstance(&tasks[i], time, ++jobCount[i]));
        }

        if (currentJob && currentJob->remainingTime <= 0) {
            free(currentJob); currentJob = NULL;
        }

        if (!isEmpty(&pq)) {
            if (currentJob == NULL || pq.jobs[0]->currentPriority < currentJob->currentPriority) {
                if (currentJob) insertJob(&pq, currentJob);
                currentJob = extractMin(&pq);
            }
        }

        if (currentJob == NULL) {
            printf("%2d to %-3d %-8s %-10s %-15s\n", time, time + 1, "IDLE", "-", "-");
            time++; continue;
        }

        Task* tDef = findTaskById(tasks, taskCount, currentJob->taskId);
        if (currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == ru.startTime && currentJob->holdingResourceId == -1) {
                if (!lockResourcePIP(&resources[ru.resourceId - 1], (struct Job*)currentJob, (struct PriorityQueue*)&pq)) {
                    insertJob(&pq, currentJob);
                    currentJob = extractMin(&pq);
                }
            }
        }

        char resBuf[10], inhBuf[10], jobBuf[10];
        sprintf(jobBuf, "J%d%d", currentJob->taskId, currentJob->jobNumber);
        
        if (currentJob->holdingResourceId != -1) sprintf(resBuf, "R%d", currentJob->holdingResourceId);
        else sprintf(resBuf, "-");
        
        if (currentJob->inheritedFromTask != -1) sprintf(inhBuf, "T%d", currentJob->inheritedFromTask);
        else sprintf(inhBuf, "None");

        printf("%2d to %-3d %-8s %-10s %-15s\n", time, time+1, jobBuf, resBuf, inhBuf);

        currentJob->remainingTime--; currentJob->executedTime++; time++;

        if (currentJob && currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == (ru.startTime + ru.duration) && currentJob->holdingResourceId != -1) {
                unlockResourcePIP(&resources[ru.resourceId - 1], (struct PriorityQueue*)&pq);
                currentJob->resourceIndex++;
            }
        }
    }
}

// 2. PCP SCHEDULER

void runPCPScheduler(Task tasks[], int taskCount, int resourceCount) {
    int time = 0;
    int hyperperiod = tasks[0].period;
    for (int i = 1; i < taskCount; i++) hyperperiod = get_lcm(hyperperiod, tasks[i].period);

    PriorityQueue pq; 
    initQueue(&pq);
    Resource resources[10];

    for (int i = 0; i < resourceCount; i++) {
        initResource(&resources[i], i + 1);
        int highestPriorityUsing = INT_MAX; 
        for (int j = 0; j < taskCount; j++) {
            for (int k = 0; k < tasks[j].resourceCount; k++) {
                if (tasks[j].resources[k].resourceId == resources[i].resourceId) {
                    if (tasks[j].deadline < highestPriorityUsing) highestPriorityUsing = tasks[j].deadline;
                }
            }
        }
        resources[i].priorityCeiling = highestPriorityUsing;
    }

    Job* currentJob = NULL;
    int jobCount[10] = {0};

 
    printf("%-10s %-8s %-10s %-15s\n", "Interval", "Job", "Resource", "Ceiling Block?");
    printf("----------------------------------------------------------\n");

    while (time < hyperperiod) {
        for (int i = 0; i < taskCount; i++) {
            if (time % tasks[i].period == 0) 
                insertJob(&pq, createJobInstance(&tasks[i], time, ++jobCount[i]));
        }

        if (currentJob && currentJob->remainingTime <= 0) {
            free(currentJob); currentJob = NULL;
        }

        if (!isEmpty(&pq)) {
            if (currentJob == NULL || pq.jobs[0]->currentPriority < currentJob->currentPriority) {
                if (currentJob) insertJob(&pq, currentJob);
                currentJob = extractMin(&pq);
            }
        }

        if (currentJob == NULL) {
            printf("%2d to %-3d %-8s %-10s %-15s\n", time, time + 1, "IDLE", "-", "-");
            time++; continue;
        }

        int wasCeilingBlocked = 0;
        Task* tDef = findTaskById(tasks, taskCount, currentJob->taskId);
        if (currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            
            if (currentJob->executedTime == ru.startTime && currentJob->holdingResourceId == -1) {
                int targetResIdx = ru.resourceId - 1;
                if (resources[targetResIdx].isLocked) {
                    wasCeilingBlocked = 0; 
                } else {
                    wasCeilingBlocked = 1; 
                }

                if (!lockResourcePCP(resources, resourceCount, targetResIdx, (struct Job*)currentJob, (struct PriorityQueue*)&pq)) {
                    insertJob(&pq, currentJob);
                    currentJob = extractMin(&pq);
                } else {
                    wasCeilingBlocked = 0; 
                }
            }
        }

        char resBuf[10], jobBuf[10];
        sprintf(jobBuf, "J%d%d", currentJob->taskId, currentJob->jobNumber);
        if (currentJob->holdingResourceId != -1) sprintf(resBuf, "R%d", currentJob->holdingResourceId);
        else sprintf(resBuf, "-");

        printf("%2d to %-3d %-8s %-10s %-15s\n", 
               time, time + 1, jobBuf, resBuf, (wasCeilingBlocked ? "YES" : "No"));

        currentJob->remainingTime--; currentJob->executedTime++; time++;

        if (currentJob && currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == (ru.startTime + ru.duration) && currentJob->holdingResourceId != -1) {
                unlockResourcePCP(&resources[ru.resourceId - 1], (struct PriorityQueue*)&pq);
                currentJob->resourceIndex++;
            }
        }
    }
}