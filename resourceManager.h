#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "task.h"


typedef enum {
    PROTOCOL_NONE,
    PROTOCOL_PIP, 
    PROTOCOL_PCP
} ProtocolType;

typedef struct Resource {
    int resourceId;
    int isLocked;
    struct Job* ownerJob;  
    int priorityCeiling;   
} Resource;

struct PriorityQueue;

void initResource(Resource* r, int id);

int lockResourcePIP(Resource* r, struct Job* requester, struct PriorityQueue* pq);
void unlockResourcePIP(Resource* r, struct PriorityQueue* pq);
int lockResourcePCP(Resource* resArray, int resCount, int targetIdx, struct Job* requester, struct PriorityQueue* pq);
void unlockResourcePCP(Resource* r, struct PriorityQueue* pq);

#endif