#include "priorityQueue.h"
#include <stddef.h>

#include <stdio.h>


void initQueue(PriorityQueue* pq) 
{
    pq->size = 0;
}

int isEmpty(PriorityQueue* pq) 
{
    return pq->size == 0;
}


void swap(Job** a, Job** b) 
{
    Job* t = *a;
    *a = *b;
    *b = t;
}


int higherPriority(Job* a, Job* b) 
{

    if (a->absoluteDeadline < b->absoluteDeadline) return 1;

    if (a->absoluteDeadline > b->absoluteDeadline) return 0;

    if (a->taskId < b->taskId) return 1;          // both task has same deadline.

    if (a->taskId > b->taskId) return 0;         // same deadline.

    return a->jobNumber < b->jobNumber;    // means there are two jobs of same task released at same time

}


void heapifyUp(PriorityQueue* pq, int i) {

    while (i > 0) {
        int p = (i - 1) / 2;

        if (higherPriority(pq->jobs[i], pq->jobs[p])) {
            swap(&pq->jobs[i], &pq->jobs[p]);
            i = p;
        } else break;
    }
}


void heapifyDown(PriorityQueue* pq, int i) {

    while (1) {
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        int smallest = i;

        if (l < pq->size && higherPriority(pq->jobs[l], pq->jobs[smallest]))
            smallest = l;

        if (r < pq->size && higherPriority(pq->jobs[r], pq->jobs[smallest]))
            smallest = r;

        if (smallest != i) {
            swap(&pq->jobs[i], &pq->jobs[smallest]);
            i = smallest;
        } else break;
    }
}

// ---------- INSERT ----------
void insertJob(PriorityQueue* pq, Job* job) {

    pq->jobs[pq->size] = job;
    heapifyUp(pq, pq->size);
    pq->size++;
}

// ---------- EXTRACT ----------
Job* extractMin(PriorityQueue* pq) {

    if (pq->size == 0) return NULL;

    Job* top = pq->jobs[0];

    pq->jobs[0] = pq->jobs[pq->size - 1];
    pq->size--;

    heapifyDown(pq, 0);

    return top;
}