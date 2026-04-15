#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "task.h"

// Defined as a struct with a typedef to match your resourceManager forward declarations
typedef struct PriorityQueue {
    Job* jobs[1000];
    int size;
} PriorityQueue;

void initQueue(PriorityQueue* pq);
void insertJob(PriorityQueue* pq, Job* job);
Job* extractMin(PriorityQueue* pq);
int isEmpty(PriorityQueue* pq);
void heapifyDown(PriorityQueue* pq, int i);
void heapifyUp(PriorityQueue* pq, int i);
void swap(Job** a, Job** b);
void removeJob(PriorityQueue* pq, int taskId, int jobNumber);
void reheapifyJob(PriorityQueue* pq, int taskId, int jobNumber);

#endif