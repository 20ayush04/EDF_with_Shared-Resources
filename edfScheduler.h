#ifndef EDF_SCHEDULER_H
#define EDF_SCHEDULER_H

#include "task.h"

void runPIPScheduler(Task tasks[], int taskCount, int resourceCount);
void runPCPScheduler(Task tasks[], int taskCount, int resourceCount);

#endif