#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "edfScheduler.h"
#include "priorityQueue.h"
#include "resourceManager.h"
#include "blockedQueue.h"
#include "task.h"

// --- Helpers ---
int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
int lcm(int a, int b) { if (a == 0 || b == 0) return 0; return (a * b) / gcd(a, b); }

int computeHyperperiod(Task t[], int n) {
    if (n == 0) return 0;
    int h = t[0].period;
    for (int i = 1; i < n; i++) h = lcm(h, t[i].period);
    return h;
}

int getNextReleaseTime(int currentTime, Task tasks[], int taskCount) {
    int next = INT_MAX;
    for (int i = 0; i < taskCount; i++) {
        int nextRel = ((currentTime / tasks[i].period) + 1) * tasks[i].period;
        if (nextRel < next) next = nextRel;
    }
    return next;
}

void runEdfScheduler(Task tasks[], int taskCount, int resourceCount, ProtocolType protocol) {
    int time = 0;
    int hyper = computeHyperperiod(tasks, taskCount);
    
    PriorityQueue pq;
    initQueue(&pq);
    BlockedQueue bq;
    initBlockedQueue(&bq);
    
    Resource resources[10];
    for (int i = 0; i < resourceCount; i++) initResource(&resources[i], i + 1);

    int jobCount[10] = {0};
    Job* currentJob = NULL; 

    printf("\n%-5s %-5s %-8s %-5s %-5s %-10s\n", "Start", "End", "Job", "Res", "Inh", "Deadline");
    printf("----------------------------------------------------------\n");

    while (time < hyper) {
        // 1. ARRIVAL: Add new jobs to PQ
        for (int i = 0; i < taskCount; i++) {
            if (time % tasks[i].period == 0) {
                Job* j = malloc(sizeof(Job));
                j->taskId = tasks[i].taskId;
                j->jobNumber = ++jobCount[i];
                j->remainingTime = tasks[i].executionTime;
                j->executedTime = 0;
                j->absoluteDeadline = time + tasks[i].deadline;
                j->currentPriority = j->absoluteDeadline;
                j->originalPriority = j->absoluteDeadline;
                j->holdingResourceId = -1;
                j->waitingResourceId = -1;
                j->resourceIndex = 0;
                j->isBlocked = 0;
                j->inheritedFromTask = -1;
                insertJob(&pq, j);
            }
        }

        tryUnblockJobs(&bq, &pq, resources);

        // 2. SELECTION: Compare currentJob vs Best in PQ
        if (currentJob == NULL || currentJob->remainingTime <= 0 || currentJob->isBlocked) {
            currentJob = extractMin(&pq);
        } else if (!isEmpty(&pq)) {
            // ONLY PREEMPT IF THE NEW JOB HAS A STRICTLY SMALLER DEADLINE
            if (pq.jobs[0]->currentPriority < currentJob->currentPriority) {
                insertJob(&pq, currentJob); // Put current back
                currentJob = extractMin(&pq); // Take the better one
            }
        }

        // 3. IDLE
        if (currentJob == NULL) {
            int nextRel = getNextReleaseTime(time, tasks, taskCount);
            if (nextRel > hyper) nextRel = hyper;
            printf("%-5d %-5d %-8s %-5s %-5s %-10s\n", time, nextRel, "IDLE", "-", "-", "-");
            time = nextRel;
            continue;
        }

        Task* t = findTaskById(tasks, taskCount, currentJob->taskId);

        // 4. CALCULATE DURATION
        int nextEvent = getNextReleaseTime(time, tasks, taskCount);
        int finishTime = time + currentJob->remainingTime;
        if (finishTime < nextEvent) nextEvent = finishTime;

        if (currentJob->resourceIndex < t->resourceCount) {
            ResourceUsage ru = t->resources[currentJob->resourceIndex];
            int reqTime = time + (ru.startTime - currentJob->executedTime);
            int relTime = time + (ru.startTime + ru.duration - currentJob->executedTime);
            if (reqTime > time && reqTime < nextEvent) nextEvent = reqTime;
            if (relTime > time && relTime < nextEvent) nextEvent = relTime;
        }
        if (nextEvent > hyper) nextEvent = hyper;

        // 5. LOCKING
        if (currentJob->resourceIndex < t->resourceCount) {
            ResourceUsage ru = t->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == ru.startTime && currentJob->holdingResourceId == -1) {
                if (!lockResourcePIP(&resources[ru.resourceId - 1], (struct Job*)currentJob, (struct PriorityQueue*)&pq)) {
                    addBlockedJob(&bq, currentJob);
                    currentJob = NULL; // Trigger re-selection next loop
                    continue; 
                }
            }
        }

        // 6. EXECUTE
        int duration = nextEvent - time;
        char jobName[10]; 
        sprintf(jobName, "J%d%d", currentJob->taskId, currentJob->jobNumber);
        
        // Temporary buffers for ID strings
        char resIdStr[10];
        char inhIdStr[10];

        // Convert Resource ID to string or "-"
        if (currentJob->holdingResourceId != -1) {
            sprintf(resIdStr, "%d", currentJob->holdingResourceId);
        } else {
            sprintf(resIdStr, "-");
        }

        // Convert Inherited Task ID to string or "-"
        if (currentJob->inheritedFromTask != -1) {
            sprintf(inhIdStr, "T%d", currentJob->inheritedFromTask);
        } else {
            sprintf(inhIdStr, "-");
        }

        // Final Corrected Print Statement
        printf("%-5d %-5d %-8s %-5s %-5s %-10d\n", 
               time, 
               nextEvent, 
               jobName, 
               resIdStr, 
               inhIdStr, 
               currentJob->currentPriority);

        currentJob->remainingTime -= duration;
        currentJob->executedTime += duration;
        time = nextEvent;

        // 7. UNLOCK
        if (currentJob->resourceIndex < t->resourceCount) {
            ResourceUsage ru = t->resources[currentJob->resourceIndex];
            if (currentJob->executedTime == (ru.startTime + ru.duration)) {
                unlockResourcePIP(&resources[ru.resourceId - 1], (struct PriorityQueue*)&pq);
                currentJob->resourceIndex++;
            }
        }
        
        // If finished, clear pointer so we pull a new one next time
        if (currentJob->remainingTime <= 0) {
            free(currentJob);
            currentJob = NULL;
        }
    }
}