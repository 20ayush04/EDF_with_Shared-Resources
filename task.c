#include "task.h"
#include <stddef.h>

Task* findTaskById(Task tasks[], int taskCount, int taskId) {
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].taskId == taskId)
            return &tasks[i];
    }
    return NULL;
}