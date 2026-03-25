#ifndef TASK_H
#define TASK_H

#define MAX_RESOURCES 5

typedef struct {
    int resourceId;
    int startTime;
    int duration;
} ResourceUsage;

typedef struct {
    int taskId;
    int executionTime;
    int period;
    int deadline;
    int resourceCount;
    ResourceUsage resources[MAX_RESOURCES];
} Task;

typedef struct {
    int taskId;
    int jobNumber;

    int remainingTime;
    int executedTime;

    int absoluteDeadline;

    int currentPriority;
    int originalPriority;

    int isBlocked;

    int resourceIndex;
    int holdingResourceId;
    int waitingResourceId;

    int inheritedFromTask;

} Job;

Task* findTaskById(Task tasks[], int taskCount, int taskId);

#endif