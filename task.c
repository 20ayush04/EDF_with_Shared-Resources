#include "task.h"
#include <stddef.h>

/**
 * Iterates through the task array to find the pointer to a specific task.
 * Used by the scheduler to look up ResourceUsage details for a running Job.
 */
Task* findTaskById(Task tasks[], int taskCount, int taskId) {
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].taskId == taskId) {
            return &tasks[i];
        }
    }
    return NULL;
}