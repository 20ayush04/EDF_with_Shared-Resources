#ifndef BLOCKED_QUEUE_H
#define BLOCKED_QUEUE_H

#include "task.h"
#include "priorityQueue.h"
#include "resourceManager.h"

#define MAX_BLOCKED 100

typedef struct {
    Job* jobs[MAX_BLOCKED];
    int size;
} BlockedQueue;

// Initializes the blocked queue to zero size
void initBlockedQueue(BlockedQueue* bq);

// Adds a job to the blocked list when a resource lock fails
void addBlockedJob(BlockedQueue* bq, Job* job);

// Checks all blocked jobs; if their required resource is free, 
// they are moved back to the PriorityQueue (Ready state)
void tryUnblockJobs(BlockedQueue* bq, PriorityQueue* pq, Resource resources[]);

#endif