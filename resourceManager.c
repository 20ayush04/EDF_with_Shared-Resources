#include <stdio.h>
#include "resourceManager.h"

// 🔹 reinsert for priority update
void reinsertJob(PriorityQueue* pq, Job* job){

    for(int i=0;i<pq->size;i++){
        if(pq->jobs[i] == job){
            pq->jobs[i] = pq->jobs[pq->size-1];
            pq->size--;
            heapifyDown(pq,i);
            break;
        }
    }
    insertJob(pq, job);
}

// 🔹 init
void initResource(Resource* r, int id){
    r->resourceId = id;
    r->isLocked = 0;
    r->lockedBy = NULL;
}

// 🔹 LOCK (PIP CORE)
int lockResourcePIP(Resource* r, Job* job, PriorityQueue* pq){

    // ✅ free → acquire
    if(!r->isLocked){
        r->isLocked = 1;
        r->lockedBy = job;

        job->holdingResourceId = r->resourceId;
        job->waitingResourceId = -1;
        job->isBlocked = 0;

        return 1;
    }

    // ❌ already locked → BLOCK + PIP
    Job* holder = r->lockedBy;

    job->isBlocked = 1;
    job->waitingResourceId = r->resourceId;

    // 🔥 PIP CONDITION
    if(holder->currentPriority > job->currentPriority){

        // inherit priority
        holder->currentPriority = job->currentPriority;
        holder->inheritedFromTask = job->taskId;

        // update position in ready queue
        reinsertJob(pq, holder);
    }

    return 0;
}

// 🔹 UNLOCK
void unlockResourcePIP(Resource* r, PriorityQueue* pq){

    if(!r->isLocked) return;

    Job* holder = r->lockedBy;

    // 🔥 restore original priority
    holder->currentPriority = holder->originalPriority;
    holder->inheritedFromTask = -1;

    // update queue again
    reinsertJob(pq, holder);

    r->isLocked = 0;
    r->lockedBy = NULL;
}