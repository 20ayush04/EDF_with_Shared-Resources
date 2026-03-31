#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H

#include "task.h"

// utilization
double computeUtilization(Task tasks[], int n);

// blocking time (common for both PIP/PCP worst-case)
int computeBlocking(Task tasks[], int n, int taskIndex);

// schedulability check
int isSchedulable(Task tasks[], int n);

#endif