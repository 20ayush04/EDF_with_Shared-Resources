#ifndef BLOCKED_QUEUE_H
#define BLOCKED_QUEUE_H

#include "task.h"

typedef struct PriorityQueue PriorityQueue;
typedef struct Resource Resource;

#define MAX_BLOCKED 100

typedef struct BlockedQueue {
    Job* jobs[MAX_BLOCKED];
    int size;
} BlockedQueue;

void initBlockedQueue(BlockedQueue* bq);
void addBlockedJob(BlockedQueue* bq, Job* job);
void tryUnblockJobs(BlockedQueue* bq, PriorityQueue* pq, struct Resource* resources);
#endif