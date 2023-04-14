//
// Created by 陶挺 on 14.04.23.
//

#include <printf.h>
#include <string.h>
#include "CSimpleThreadPool.h"

ThreadPool *init_ThreadPool(int minThreadsNum, int maxThreadsNum, int queueCapacity){
    ThreadPool* pool =(ThreadPool*) malloc(sizeof(ThreadPool));
    do{
        if(pool == NULL){
            printf("malloc pool faile\n");
            // we could use break in this while, and set while(0) so it wont loop
            //return NULL;
            break;
        }

        //define the array to store threads
        pool->maxThreadsNum = maxThreadsNum;
        pool->minThreadsNum = minThreadsNum;
        pool->queueCapacity = queueCapacity;
        pool->workingThreadsNum = 0;
        pool->liveThreadsNum = minThreadsNum;
        pool->threadsToExit = 0;

        pool->threadIDs = (pthread_t*)malloc(sizeof (pthread_t) * maxThreadsNum);
        if(pool->threadIDs == NULL){
            printf("malloc threadIDs failed\n"){
                break;
            }
        }

        //memset init this buffer to 0
        memset(pool->threadIDs,0,sizeof(pthread_t)*maxThreadsNum);

        if(pthread_mutex_init(&pool->poolMutex, NULL) != 0||pthread_mutex_init(&pool->workingThreadsNumMutex, NULL) != 0
           ||pthread_cond_init(&pool->isEmpty, NULL) != 0 || pthread_cond_init(&pool->isFull,NULL) != 0){
            printf("innit mutex falied");
            break;
        }

        pool->taskQueue = malloc(sizeof(Task) * queueCapacity);
        pool->queueSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;

        pool->shutdown = 0;

        //init threads
        if(pthread_create(&pool->managerID,NULL,manager,NULL) != 0){
            printf("manager thread creation falied");
            break;
        }
        for(int i = 0; i < minThreadsNum; i++){
            if(pthread_create(&pool->threadIDs[i], NULL, work,NULL)!=0){
                printf("working thred creation falied");
                break;
            }
        }

        return pool;
    }while(0);

    //if while breaks, we have to free the memories
    if(pool->threadIDs) free(pool->threadIDs);
    if(pool->taskQueue) free(pool->taskQueue);
    if(pool) free(pool);

    return NULL;
}