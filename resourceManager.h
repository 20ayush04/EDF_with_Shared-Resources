#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

// Forward declaration
struct Job;
struct PriorityQueue; 

typedef enum {
    PROTOCOL_NONE,
    PROTOCOL_PIP,
    PROTOCOL_PCP
} ProtocolType;

typedef struct {
    int resourceId;
    int isLocked;
    struct Job* ownerJob; 
    int priorityCeiling; 
} Resource;

void initResource(Resource* r, int id);

// Use 'struct Job' and 'struct PriorityQueue' to avoid conflicting with the typedefs later
int lockResourcePIP(Resource* r, struct Job* requester, struct PriorityQueue* pq);
void unlockResourcePIP(Resource* r, struct PriorityQueue* pq);

#endif