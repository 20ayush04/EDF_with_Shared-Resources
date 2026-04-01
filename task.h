#ifndef TASK_H
#define TASK_H

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
    ResourceUsage resources[10]; 
} Task;

typedef struct {
    int jobNumber;         
    int taskId;
    int remainingTime;      
    int executedTime;       
    int absoluteDeadline;   
    int currentPriority;     
    int originalPriority;    
    int holdingResourceId;  
    int waitingResourceId;   
    int resourceIndex;       
    int isBlocked;           
    int inheritedFromTask;   

} Job;

Task* findTaskById(Task tasks[], int n, int id);

#endif