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

/**
 * This is the "Wake Up" logic.
 * It iterates through the blocked jobs and checks if the 
 * resource they are waiting for (waitingResourceId) is now unlocked.
 */
void tryUnblockJobs(BlockedQueue* bq, PriorityQueue* pq, Resource resources[]) {
    for (int i = 0; i < bq->size; i++) {
        Job* job = bq->jobs[i];
        int rid = job->waitingResourceId;

        // Resource IDs are 1-based, so we check index rid-1
        if (rid != -1 && !resources[rid - 1].isLocked) {
            
            // 1. Mark job as no longer blocked
            job->isBlocked = 0;
            job->waitingResourceId = -1;

            // 2. Move it back to the Ready Queue (PriorityQueue)
            insertJob(pq, job);

            // 3. Remove from BlockedQueue (Swap with last element and decrement)
            bq->jobs[i] = bq->jobs[bq->size - 1];
            bq->size--;
            
            // 4. Since we swapped the current index, stay at 'i' to check the new element
            i--; 
        }
    }
}