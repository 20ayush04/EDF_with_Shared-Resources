#include "priorityQueue.h"
#include <stddef.h>

void swap(Job** a, Job** b) {
    Job* t = *a; *a = *b; *b = t;
}

void initQueue(PriorityQueue* pq) {
    pq->size = 0;
}

void heapifyUp(PriorityQueue* pq, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        // TIE-BREAKER: Only swap if the child is STRICTLY better (smaller deadline)
        // This keeps the older job (J11) at the top if J22 has the same deadline.
        if (pq->jobs[i]->currentPriority < pq->jobs[p]->currentPriority) {
            swap(&pq->jobs[p], &pq->jobs[i]);
            i = p;
        } else {
            break;
        }
    }
}

void heapifyDown(PriorityQueue* pq, int i) {
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, smallest = i;

        if (l < pq->size && pq->jobs[l]->currentPriority < pq->jobs[smallest]->currentPriority)
            smallest = l;

        if (r < pq->size && pq->jobs[r]->currentPriority < pq->jobs[smallest]->currentPriority)
            smallest = r;

        if (smallest == i) break;

        swap(&pq->jobs[i], &pq->jobs[smallest]);
        i = smallest;
    }
}

void insertJob(PriorityQueue* pq, Job* job) {
    pq->jobs[pq->size] = job;
    heapifyUp(pq, pq->size);
    pq->size++;
}

Job* extractMin(PriorityQueue* pq) {
    if (pq->size == 0) return NULL;
    Job* j = pq->jobs[0];
    pq->jobs[0] = pq->jobs[--pq->size];
    heapifyDown(pq, 0);
    return j;
}

int isEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}