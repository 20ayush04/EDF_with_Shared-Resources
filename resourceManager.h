#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "task.h"
#include "priorityQueue.h"

typedef struct {
    int resourceId;
    int isLocked;
    Job* lockedBy;
} Resource;

void initResource(Resource* r, int id);

int lockResourcePIP(Resource* r, Job* job, PriorityQueue* pq);
void unlockResourcePIP(Resource* r, PriorityQueue* pq);

#endif