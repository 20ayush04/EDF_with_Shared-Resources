#include "blockedQueue.h"
#include <stdio.h>

void initBlockedQueue(BlockedQueue* bq) {
    bq->size = 0;
}

void addBlockedJob(BlockedQueue* bq, Job* job) {
    if (bq->size < MAX_BLOCKED) {
        job->isBlocked = 1;
        bq->jobs[bq->size++] = job;
    } else {
        printf("Error: Blocked Queue Overflow!\n");
    }
}


void tryUnblockJobs(BlockedQueue* bq, PriorityQueue* pq, Resource resources[]) {
    for (int i = 0; i < bq->size; i++) {
        Job* job = bq->jobs[i];
        int rid = job->waitingResourceId;

        if (rid != -1 && !resources[rid - 1].isLocked) {
            
            job->isBlocked = 0;
            job->waitingResourceId = -1;

            insertJob(pq, job);

            bq->jobs[i] = bq->jobs[bq->size - 1];
            bq->size--;
            
            i--; 
        }
    }
}