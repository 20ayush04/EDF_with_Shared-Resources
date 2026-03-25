#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "edfScheduler.h"
#include "priorityQueue.h"
#include "task.h"
#include "resourceManager.h"
#include "blockedQueue.h"

// ---------- GCD / LCM ----------
int gcd(int a,int b){ return b==0?a:gcd(b,a%b); }
int lcm(int a,int b){ return (a*b)/gcd(a,b); }

int computeHyperperiod(Task t[],int n){
    // printf("Computing hyperperiod...\n");
    int h=t[0].period;
    for(int i=1;i<n;i++) 
    {
        h=lcm(h,t[i].period);
    }
    printf("%d\n",h);
    return h;
}

// ---------- NEXT RELEASE ----------
int findNextReleaseTime(int time, Task tasks[], int n){
    int next = INT_MAX;
    for(int i=0;i<n;i++){
        int k = (time / tasks[i].period) + 1;
        int rel = k * tasks[i].period;
        if(rel < next) next = rel;
    }
    return next;
}

// ---------- PRINT ----------
void printGantt(int s,int e,Job* job,char* res){
    if(job == NULL){
        printf("%-5d %-5d %-6s %-6s\n",s,e,"IDLE","-");
        return;
    }
    char name[10];
    sprintf(name,"J%d%d",job->taskId,job->jobNumber);
    printf("%-5d %-5d %-6s %-6s\n",s,e,name,res);
}

    
void runEdfScheduler(Task tasks[], int taskCount, int resourceCount){
        // printf("\nScheduler started\n");

    int time = 0;
    int hyper = computeHyperperiod(tasks,taskCount);

            // printf("\nScheduler started\n");

    PriorityQueue pq; initQueue(&pq);
    BlockedQueue bq; initBlockedQueue(&bq);
            // printf("\nScheduler started\n");


    Resource resources[5];
    for(int i=0;i<resourceCount;i++)
        initResource(&resources[i],i+1);
                // printf("\nScheduler started\n");


    int jobCount[100]={0};

    printf("Start End  Job    Res\n");
    printf("--------------------------\n");

    while(time < hyper){
        printf("Time: %d\n",time);
        // 🔹 RELEASE JOBS
        for(int i=0;i<taskCount;i++){
            if(time % tasks[i].period == 0){

                Job* job = malloc(sizeof(Job));

                jobCount[i]++;
                job->jobNumber = jobCount[i];
                job->taskId = tasks[i].taskId;

                job->remainingTime = tasks[i].executionTime;
                job->executedTime = 0;

                job->absoluteDeadline = time + tasks[i].deadline;
                job->currentPriority = job->absoluteDeadline;

                job->resourceIndex = 0;
                job->holdingResourceId = -1;

                insertJob(&pq, job);
                // printf("inserted job J%d%d at time %d with deadline %d\n",job->taskId,job->jobNumber,time,job->absoluteDeadline);
            }
        }

        // 🔹 UNBLOCK
        tryUnblockJobs(&bq, &pq, resources);

        // 🔹 IDLE
        if(isEmpty(&pq)){
            int next = findNextReleaseTime(time,tasks,taskCount);
            if(next <= time) next = time + 1;
            printGantt(time,next,NULL,"-");
            time = next;
            continue;
        }

        // 🔹 PICK JOB
        Job* job = extractMin(&pq);
        Task* t = findTaskById(tasks,taskCount,job->taskId);

        char res[10] = "-";

        // 🔥 COMPUTE NEXT EVENT CORRECTLY
        int nextEvent = INT_MAX;

        // 1. next release
        int nextRelease = findNextReleaseTime(time,tasks,taskCount);
        if(nextRelease > time && nextRelease < nextEvent)
            nextEvent = nextRelease;

        // 2. job finish
        int finishTime = time + job->remainingTime;
        if(finishTime < nextEvent)
            nextEvent = finishTime;

        // 3. resource events
        if(job->resourceIndex < t->resourceCount){

            ResourceUsage ru = t->resources[job->resourceIndex];

            int startEvent = time + (ru.startTime - job->executedTime);
            int endEvent   = time + ((ru.startTime + ru.duration) - job->executedTime);

            if(startEvent > time && startEvent < nextEvent)
                nextEvent = startEvent;

            if(endEvent > time && endEvent < nextEvent)
                nextEvent = endEvent;
        }

        // 🔥 SAFETY
        if(nextEvent == INT_MAX || nextEvent <= time)
            nextEvent = time + 1;

        int runTime = nextEvent - time;

        // 🔹 EXECUTE
        job->remainingTime -= runTime;
        job->executedTime += runTime;

        // 🔹 RESOURCE ACQUIRE
        if(job->resourceIndex < t->resourceCount){

            ResourceUsage ru = t->resources[job->resourceIndex];

            if(job->executedTime >= ru.startTime &&
               job->holdingResourceId == -1){

                if(!resources[ru.resourceId-1].isLocked){
                    resources[ru.resourceId-1].isLocked = 1;
                    resources[ru.resourceId-1].lockedBy = job;
                    job->holdingResourceId = ru.resourceId;
                }
            }
        }

        if(job->holdingResourceId != -1)
            sprintf(res,"R%d",job->holdingResourceId);

        printGantt(time,nextEvent,job,res);

        time = nextEvent;

        // 🔹 RESOURCE RELEASE
        if(job->resourceIndex < t->resourceCount){

            ResourceUsage ru = t->resources[job->resourceIndex];

            if(job->executedTime >= ru.startTime + ru.duration &&
               job->holdingResourceId == ru.resourceId){

                resources[ru.resourceId-1].isLocked = 0;
                resources[ru.resourceId-1].lockedBy = NULL;

                job->holdingResourceId = -1;
                job->resourceIndex++;
            }
        }

        // 🔹 FINISH OR REINSERT
        if(job->remainingTime > 0)
            insertJob(&pq, job);
        else
            free(job);
    }
}