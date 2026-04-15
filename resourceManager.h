#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "task.h"

typedef struct PriorityQueue PriorityQueue;
typedef struct BlockedQueue BlockedQueue;

typedef enum {
    PROTOCOL_NONE,
    PROTOCOL_PIP, 
    PROTOCOL_PCP
} ProtocolType;

typedef struct Resource {
    int resourceId;
    int isLocked;
    Job* ownerJob;  
    int priorityCeiling;   
} Resource;

void initResource(Resource* r, int id);

int lockResourcePIP(Resource* r, Job* requester, PriorityQueue* pq, BlockedQueue* bq);
void unlockResourcePIP(Resource* r, PriorityQueue* pq);



int lockResourcePCP(Resource* resArray, int resCount, int targetIdx, Job* requester, PriorityQueue* pq);
void unlockResourcePCP(Resource* r, PriorityQueue* pq);

#endif