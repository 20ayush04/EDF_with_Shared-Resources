#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "task.h"

#define MAX_JOBS 1000

typedef struct {
    Job* jobs[MAX_JOBS];
    int size;
} PriorityQueue;

void initQueue(PriorityQueue* pq);
void insertJob(PriorityQueue* pq, Job* job);
Job* extractMin(PriorityQueue* pq);
int isEmpty(PriorityQueue* pq);

#endif