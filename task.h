#ifndef TASK_H
#define TASK_H

// Structure to define how a task uses a resource
typedef struct {
    int resourceId;
    int startTime;   // Relative to the start of the job execution
    int duration;    // How long the resource is held
} ResourceUsage;

// Static definition of a Task (from input.txt)
typedef struct {
    int taskId;
    int executionTime;
    int period;
    int deadline;

    int resourceCount;
    ResourceUsage resources[10]; // Max 10 resource usages per task
} Task;

// Dynamic instance of a Task (a Job)
typedef struct {
    int jobNumber;           // Instance count (e.g., J11, J12)
    int taskId;

    int remainingTime;       // Total time left to complete
    int executedTime;        // Total time already spent on CPU

    int absoluteDeadline;    // Arrival Time + Task Deadline

    int currentPriority;     // In EDF, this is the current Absolute Deadline (can be boosted)
    int originalPriority;    // The original Absolute Deadline (to restore after PIP)

    int holdingResourceId;   // ID of resource currently held (-1 if none)
    int waitingResourceId;   // ID of resource the job is blocked on (-1 if none)

    int resourceIndex;       // Which resource usage from the Task struct is next/current

    int isBlocked;           // 1 if waiting for a resource, 0 otherwise
    int inheritedFromTask;   // ID of the high-priority task providing the boost (-1 if none)

} Job;

// Helper to find task metadata by ID
Task* findTaskById(Task tasks[], int n, int id);

#endif