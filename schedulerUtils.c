#include <stdio.h>
#include "schedulerUtils.h"
#include "task.h"

double computeUtilization(Task t[], int n){
    double u = 0;
    for(int i=0;i<n;i++)
    {
        u += (double)t[i].executionTime / t[i].period;
    }
    return u;
}

int computeBlocking(Task t[], int n, int i){
    int Bi = 0;
    for(int j=0;j<n;j++){
        if(t[j].deadline > t[i].deadline){
            for(int k=0;k<t[j].resourceCount;k++){
                if(t[j].resources[k].duration > Bi)
                    Bi = t[j].resources[k].duration;
            }
        }
    }
    return Bi;
}

int isSchedulable(Task t[], int n){

    double U = computeUtilization(t,n);

    if(U > 1){
        printf("Not schedulable (U>1)\n");
        return 0;
    }

    for(int i=0;i<n;i++){

        int Bi = computeBlocking(t,n,i);

        printf("Task %d Blocking = %d\n", t[i].taskId, Bi);

        if(t[i].executionTime + Bi > t[i].deadline){
            printf("Fails EDF condition\n");
            return 0;
        }
    }

    printf("Schedulable\n");
    return 1;
}