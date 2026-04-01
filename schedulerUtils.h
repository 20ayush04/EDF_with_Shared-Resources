#ifndef SCHEDULER_UTILS_H
#define SCHEDULER_UTILS_H

#include "task.h"

double computeUtilization(Task tasks[], int n);

int computeBlocking(Task tasks[], int n, int taskIndex);

int isSchedulable(Task tasks[], int n);

#endif