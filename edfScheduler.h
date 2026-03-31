#ifndef EDF_SCHEDULER_H
#define EDF_SCHEDULER_H

#include "task.h"
#include "resourceManager.h"

// main scheduler
void runEdfScheduler(Task tasks[], int taskCount, int resourceCount, ProtocolType protocol);

#endif