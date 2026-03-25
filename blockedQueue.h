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

void initBlockedQueue(BlockedQueue* bq);
void addBlockedJob(BlockedQueue* bq, Job* job);
void tryUnblockJobs(BlockedQueue* bq, PriorityQueue* pq, Resource resources[]);

#endif