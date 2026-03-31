#include "resourceManager.h"
#include "task.h"
#include "priorityQueue.h"
#include <stdio.h>

void initResource(Resource* r, int id) {
    r->resourceId = id;
    r->isLocked = 0;
    r->ownerJob = NULL;
}

int lockResourcePIP(Resource* r, struct Job* requester, struct PriorityQueue* pq) {
    Job* req = (Job*)requester;
    if (!r->isLocked) {
        r->isLocked = 1;
        r->ownerJob = requester;
        req->holdingResourceId = r->resourceId;
        return 1;
    } else {
        Job* owner = (Job*)r->ownerJob;
        // Inheritance: Boost owner to requester's deadline
        if (owner->currentPriority > req->currentPriority) {
            owner->currentPriority = req->currentPriority;
            owner->inheritedFromTask = req->taskId;
        }
        req->isBlocked = 1;
        req->waitingResourceId = r->resourceId;
        return 0;
    }
}

void unlockResourcePIP(Resource* r, struct PriorityQueue* pq) {
    if (r->ownerJob == NULL) return;
    Job* owner = (Job*)r->ownerJob;
    owner->currentPriority = owner->originalPriority;
    owner->inheritedFromTask = -1;
    owner->holdingResourceId = -1;
    r->isLocked = 0;
    r->ownerJob = NULL;
}