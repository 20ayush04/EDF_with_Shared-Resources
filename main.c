#include "task.h"
#include "inputParser.h"
#include "edfScheduler.h"

int main() {

    Task tasks[100];
    int resourceCount;

    int taskCount = readInput("input.txt", tasks, &resourceCount);

    runEdfScheduler(tasks, taskCount, resourceCount);

    return 0;
}