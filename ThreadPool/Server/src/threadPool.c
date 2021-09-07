#include "../include/head.h"
#include "../include/workque.h"
#include "../include/threadPool.h"

void cleanFunc(void* p)
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)p;

    pthread_mutex_unlock(mutex);
}

void* threadFunc(void* p)
{
    pQue_t pQue = (pQue_t)p;

    pNode_t pCur = NULL;
    int getSuccess = 1;
    while(1){
        pthread_mutex_lock(&pQue->mutex);
        pthread_cleanup_push(cleanFunc, &pQue->mutex);

        if(0 == pQue->size){
            pthread_cond_wait(&pQue->cond, &pQue->mutex);
        }
        
        getSuccess = queGet(pQue, &pCur);
        pthread_mutex_unlock(&pQue->mutex);

        if(0 == getSuccess){
            transFile(pCur->clientFd);
            free(pCur);
            pCur = NULL;
        }
        pthread_cleanup_pop(1);
    }

    pthread_exit(NULL);
}

int threadPoolInit(pThreadPool_t pPool, int threadNum)
{
    pPool->threadNum = threadNum;
    pPool->pthid = (pthread_t*)calloc(threadNum, sizeof(pthread_t));

    queInit(&pPool->Que);
    return 0;
}

int threadPoolStart(pThreadPool_t pPool)
{
    for(int i = 0; i < pPool->threadNum; ++i){
        /* pthread_create(pPool->pthid + i, NULL, threadFunc, &pPool->Que); */
        pthread_create(&pPool->pthid[i], NULL, threadFunc, &pPool->Que);
    }

    return 0;
}
