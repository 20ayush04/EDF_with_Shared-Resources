#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "edfScheduler.h"
#include "priorityQueue.h"
#include "blockedQueue.h"
#include "resourceManager.h"
#include "task.h"

// Helper functions for GCD and LCM
int get_gcd(int a, int b) {
    while (b) {
        a %= b;
        int t = a;
        a = b;
        b = t;
    }
    return a;
}

int get_lcm(int a, int b) {
    if (a == 0 || b == 0) return 0;
    return (a * b) / get_gcd(a, b);
}

Job *createJobInstance(Task *t, int arrivalTime, int jobNumber) {
    Job *j = (Job *)malloc(sizeof(Job));
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
    for (int i = 1; i < taskCount; i++)
        hyperperiod = get_lcm(hyperperiod, tasks[i].period);

    PriorityQueue pq;
    initQueue(&pq);
    BlockedQueue bq;
    initBlockedQueue(&bq);
    Resource resources[10];
    for (int i = 0; i < resourceCount; i++)
        initResource(&resources[i], i + 1);

    Job *currentJob = NULL;
    int jobCount[10] = {0};

    printf("%-10s %-8s %-10s %-15s\n", "Interval", "Job", "Resource", "Inherited From");
    printf("----------------------------------------------------------\n");

    while (time < hyperperiod) {
        for (int i = 0; i < taskCount; i++) {
            if (time % tasks[i].period == 0) {
                insertJob(&pq, createJobInstance(&tasks[i], time, ++jobCount[i]));
                printf("Time %d: Task %d releases Job %d%d\n", time, tasks[i].taskId, tasks[i].taskId, jobCount[i]);
            }
        }

        if (currentJob && currentJob->remainingTime <= 0) {
            free(currentJob);
            currentJob = NULL;
        }

        if (!isEmpty(&pq)) {
            if (currentJob == NULL || pq.jobs[0]->currentPriority < currentJob->currentPriority) {
                if (currentJob) insertJob(&pq, currentJob);
                currentJob = extractMin(&pq);
            }
        }

        int nextArrival = hyperperiod;
        for (int i = 0; i < taskCount; i++) {
            int arrival = ((time / tasks[i].period) + 1) * tasks[i].period;
            if (arrival < nextArrival) nextArrival = arrival;
        }

        if (currentJob == NULL) {
            int delta = nextArrival - time;
            printf("%2d to %-3d %-8s %-10s %-15s\n", time, time + delta, "IDLE", "-", "-");
            time += delta;
            continue;
        }

        Task *tDef = findTaskById(tasks, taskCount, currentJob->taskId);
        int delta = currentJob->remainingTime; 

        if (currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            
            if (currentJob->holdingResourceId == -1) {
                int timeToRequest = ru.startTime - currentJob->executedTime;
                if (timeToRequest > 0 && timeToRequest < delta) delta = timeToRequest;
            } else {
                int timeToRelease = (ru.startTime + ru.duration) - currentJob->executedTime;
                if (timeToRelease > 0 && timeToRelease < delta) delta = timeToRelease;
            }
        }

        if (nextArrival - time < delta) delta = nextArrival - time;

        if (currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == ru.startTime && currentJob->holdingResourceId == -1) {
                printf("Time %d: Job %d%d requests Resource R%d\n", time, currentJob->taskId, currentJob->jobNumber, ru.resourceId);
                
                if (!lockResourcePIP(&resources[ru.resourceId - 1], currentJob, &pq, &bq)) {
                    printf("Time %d: Job %d%d is blocked waiting for Resource R%d\n", time, currentJob->taskId, currentJob->jobNumber, ru.resourceId);
                    currentJob = (!isEmpty(&pq)) ? extractMin(&pq) : NULL;
                    continue; 
                }
            }
        }

        char resBuf[10], inhBuf[10], jobBuf[10];
        sprintf(jobBuf, "J%d%d", currentJob->taskId, currentJob->jobNumber);
        sprintf(resBuf, (currentJob->holdingResourceId != -1) ? "R%d" : "-", currentJob->holdingResourceId);
        sprintf(inhBuf, (currentJob->inheritedFromTask != -1) ? "T%d" : "None", currentJob->inheritedFromTask);

        printf("%2d to %-3d %-8s %-10s %-15s\n", time, time + delta, jobBuf, resBuf, inhBuf);

        currentJob->remainingTime -= delta;
        currentJob->executedTime += delta;
        time += delta;

        if (currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == (ru.startTime + ru.duration) && currentJob->holdingResourceId != -1) {
                printf("Time %d: Job %d%d releases Resource R%d\n", time, currentJob->taskId, currentJob->jobNumber, ru.resourceId);
                
                unlockResourcePIP(&resources[ru.resourceId - 1], &pq);
                tryUnblockJobs(&bq, &pq, resources);
                currentJob->resourceIndex++;
            }
        }
    }
}

// 2. PCP SCHEDULER
void runPCPScheduler(Task tasks[], int taskCount, int resourceCount) {
    int time = 0;
    int hyperperiod = tasks[0].period;
    for (int i = 1; i < taskCount; i++)
        hyperperiod = get_lcm(hyperperiod, tasks[i].period);

    PriorityQueue pq;
    initQueue(&pq);
    Resource resources[10];

    for (int i = 0; i < resourceCount; i++) {
        initResource(&resources[i], i + 1);
        int highestPriorityUsing = INT_MAX;
        for (int j = 0; j < taskCount; j++) {
            for (int k = 0; k < tasks[j].resourceCount; k++) {
                if (tasks[j].resources[k].resourceId == resources[i].resourceId) {
                    if (tasks[j].deadline < highestPriorityUsing)
                        highestPriorityUsing = tasks[j].deadline;
                }
            }
        }
        resources[i].priorityCeiling = highestPriorityUsing;
    }

    Job *currentJob = NULL;
    int jobCount[10] = {0};

    printf("%-10s %-8s %-10s %-15s\n", "Interval", "Job", "Resource", "Ceiling Block?");
    printf("----------------------------------------------------------\n");

    while (time < hyperperiod) {
        for (int i = 0; i < taskCount; i++) {
            if (time % tasks[i].period == 0)
                insertJob(&pq, createJobInstance(&tasks[i], time, ++jobCount[i]));
        }

        if (currentJob && currentJob->remainingTime <= 0) {
            free(currentJob);
            currentJob = NULL;
        }

        if (!isEmpty(&pq)) {
            if (currentJob == NULL || pq.jobs[0]->currentPriority < currentJob->currentPriority) {
                if (currentJob) insertJob(&pq, currentJob);
                currentJob = extractMin(&pq);
            }
        }

        if (currentJob == NULL) {
            printf("%2d to %-3d %-8s %-10s %-15s\n", time, time + 1, "IDLE", "-", "-");
            time++;
            continue;
        }

        int wasCeilingBlocked = 0;
        Task *tDef = findTaskById(tasks, taskCount, currentJob->taskId);
        if (currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == ru.startTime && currentJob->holdingResourceId == -1) {
                int targetResIdx = ru.resourceId - 1;
                
                wasCeilingBlocked = resources[targetResIdx].isLocked ? 0 : 1;

                if (!lockResourcePCP(resources, resourceCount, targetResIdx, currentJob, &pq)) {
                    insertJob(&pq, currentJob);
                    currentJob = extractMin(&pq);
                } else {
                    wasCeilingBlocked = 0;
                }
            }
        }

        char resBuf[10], jobBuf[10];
        sprintf(jobBuf, "J%d%d", currentJob->taskId, currentJob->jobNumber);
        sprintf(resBuf, (currentJob->holdingResourceId != -1) ? "R%d" : "-", currentJob->holdingResourceId);

        printf("%2d to %-3d %-8s %-10s %-15s\n", time, time + 1, jobBuf, resBuf, (wasCeilingBlocked ? "YES" : "No"));

        currentJob->remainingTime--;
        currentJob->executedTime++;
        time++;

        if (currentJob && currentJob->resourceIndex < tDef->resourceCount) {
            ResourceUsage ru = tDef->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == (ru.startTime + ru.duration) && currentJob->holdingResourceId != -1) {
                unlockResourcePCP(&resources[ru.resourceId - 1], &pq);
                currentJob->resourceIndex++;
            }
        }
    }
}