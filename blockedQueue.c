#include "blockedQueue.h"
#include "priorityQueue.h" // Needed for insertJob
#include "resourceManager.h" // Needed for Resource struct definition
#include <stdio.h>

void initBlockedQueue(BlockedQueue* bq) {
    bq->size = 0;
}

void addBlockedJob(BlockedQueue* bq, Job* job) {
    // Check if job is already in the blocked queue to prevent duplicates
    for (int i = 0; i < bq->size; i++) {
        if (bq->jobs[i] == job) return; 
    }

    if (bq->size < MAX_BLOCKED) {
        job->isBlocked = 1;
        printf("Adding job %d%d to blocked queue (waiting for resource R%d)\n", 
                job->taskId, job->jobNumber, job->waitingResourceId);
        bq->jobs[bq->size++] = job;
    } else {
        printf("Error: Blocked Queue Overflow! Cannot add job %d%d\n", 
                job->taskId, job->jobNumber);
    }
}

void tryUnblockJobs(BlockedQueue* bq, PriorityQueue* pq, Resource resources[]) {
    for (int i = 0; i < bq->size; i++) {
        Job* job = bq->jobs[i];
        int rid = job->waitingResourceId;

        // If the resource is now free, unblock the job
        if (rid != -1 && !resources[rid - 1].isLocked) {
            job->isBlocked = 0;
            job->waitingResourceId = -1;

            // Move back to priority queue for scheduling
            insertJob(pq, job);

            // Efficient removal: swap with last element
            bq->jobs[i] = bq->jobs[bq->size - 1];
            bq->size--;
            
            // Re-check the same index since a new job was moved here
            i--; 
        }
    }
}