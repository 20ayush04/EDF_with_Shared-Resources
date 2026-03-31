#ifndef PQ_H
#define PQ_H

#include "task.h"

typedef struct {
    Job* jobs[1000];
    int size;
} PriorityQueue;

void initQueue(PriorityQueue* pq);
void insertJob(PriorityQueue* pq, Job* job);
Job* extractMin(PriorityQueue* pq);
int isEmpty(PriorityQueue* pq);

// Internal heap functions
void heapifyDown(PriorityQueue* pq, int i);
void heapifyUp(PriorityQueue* pq, int i);
void swap(Job** a, Job** b);

#endif