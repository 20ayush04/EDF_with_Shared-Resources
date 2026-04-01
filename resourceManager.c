#include "resourceManager.h"
#include "task.h"
#include "priorityQueue.h"
#include <stdio.h>
#include <limits.h>


void initResource(Resource* r, int id) {
    r->resourceId = id;
    r->isLocked = 0;
    r->ownerJob = NULL;
    r->priorityCeiling = INT_MAX; 
}

int lockResourcePIP(Resource* r, struct Job* requester, struct PriorityQueue* pq) {
    Job* req = (Job*)requester;

    if (!r->isLocked) {
        // Resource is free, take it
        r->isLocked = 1;
        r->ownerJob = requester;
        req->holdingResourceId = r->resourceId;
        return 1;
    } else {
        Job* owner = (Job*)r->ownerJob;
        
        if (req->currentPriority < owner->currentPriority) {
            owner->currentPriority = req->currentPriority;
            owner->inheritedFromTask = req->taskId;
        }
        
        req->isBlocked = 1;
        req->waitingResourceId = r->resourceId;
        return 0; // Lock failed
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

int lockResourcePCP(Resource* resArray, int resCount, int targetIdx, struct Job* requester, struct PriorityQueue* pq) {
    Job* req = (Job*)requester;
    Resource* target = &resArray[targetIdx];

    if (target->isLocked) {
        Job* owner = (Job*)target->ownerJob;
        if (req->currentPriority < owner->currentPriority) {
            owner->currentPriority = req->currentPriority;
            owner->inheritedFromTask = req->taskId;
        }
        req->isBlocked = 1;
        return 0;
    }

    int highestSystemCeiling = INT_MAX;
    Job* ceilingOwner = NULL;

    for (int i = 0; i < resCount; i++) {
        if (resArray[i].isLocked) { 
            if (resArray[i].priorityCeiling < highestSystemCeiling) {
                highestSystemCeiling = resArray[i].priorityCeiling;
                ceilingOwner = (Job*)resArray[i].ownerJob;
            }
        }
    }

   
    if (req->currentPriority < highestSystemCeiling || (ceilingOwner == req)) {
        target->isLocked = 1;
        target->ownerJob = (struct Job*)requester;
        req->holdingResourceId = target->resourceId;
        return 1;
    } else {
        if (ceilingOwner && req->currentPriority < ceilingOwner->currentPriority) {
            ceilingOwner->currentPriority = req->currentPriority;
            ceilingOwner->inheritedFromTask = req->taskId;
        }
        req->isBlocked = 1;
        return 0; 
    }
}

void unlockResourcePCP(Resource* r, struct PriorityQueue* pq) {
    unlockResourcePIP(r, pq);
}