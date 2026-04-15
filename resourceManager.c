#include "resourceManager.h"
#include "task.h"
#include "priorityQueue.h"
#include "blockedQueue.h"
#include <stdio.h>
#include <limits.h>

void initResource(Resource* r, int id) {
    r->resourceId = id;
    r->isLocked = 0;
    r->ownerJob = NULL;
    r->priorityCeiling = INT_MAX; 
}

int lockResourcePIP(Resource* r, Job* requester, PriorityQueue* pq, BlockedQueue* bq){
    Job* req = (Job*)requester;

    if (!r->isLocked) {
        r->isLocked = 1;
        r->ownerJob = requester;
        req->holdingResourceId = r->resourceId;
        return 1;
    } else {
        Job* owner = (Job*)r->ownerJob;
        printf("resource %d is locked by job %d%d, requester is job %d%d\n", r->resourceId, owner->taskId, owner->jobNumber, req->taskId, req->jobNumber);
        
        if (req->currentPriority < owner->currentPriority) {
            owner->currentPriority = req->currentPriority;
            owner->inheritedFromTask = req->taskId;
            reheapifyJob(pq, owner->taskId, owner->jobNumber); 
        }
        
        req->waitingResourceId = r->resourceId;
        addBlockedJob(bq, req); 
        return 0; 
    }
}

void unlockResourcePIP(Resource* r, PriorityQueue* pq) {
    if (r->ownerJob == NULL) return;
    
    Job* owner = (Job*)r->ownerJob;
    
    owner->currentPriority = owner->originalPriority;
    owner->inheritedFromTask = -1;
    owner->holdingResourceId = -1;
    
    r->isLocked = 0;
    r->ownerJob = NULL;

    reheapifyJob(pq, owner->taskId, owner->jobNumber);
}

// Fixed arguments to use PriorityQueue* instead of struct PriorityQueue*
int lockResourcePCP(Resource* resArray, int resCount, int targetIdx, Job* requester, PriorityQueue* pq) {
    Job* req = (Job*)requester;
    Resource* target = &resArray[targetIdx];

    if (target->isLocked) {
        Job* owner = (Job*)target->ownerJob;
        if (req->currentPriority < owner->currentPriority) {
            owner->currentPriority = req->currentPriority;
            owner->inheritedFromTask = req->taskId;
            reheapifyJob(pq, owner->taskId, owner->jobNumber);
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
        target->ownerJob = requester;
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

// Fixed arguments to use PriorityQueue* instead of struct PriorityQueue*
void unlockResourcePCP(Resource* r, PriorityQueue* pq) {
    unlockResourcePIP(r, pq);
}