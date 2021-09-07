#include "../include/head.h"
#include "../include/workque.h"

int queInit(pQue_t pQue)
{
    pQue->pHead = NULL;
    pQue->pTail = NULL;

    pQue->size = 0;

    pthread_mutex_init(&pQue->mutex, NULL);
    pthread_cond_init(&pQue->cond, NULL);

    return 0;
}

int queInsert(pQue_t pQue, pNode_t pNew)
{
    if(pQue->pTail == NULL){
        pQue->pHead = pNew;
        pQue->pTail = pNew;
    }

    else{
        pQue->pTail->pNext = pNew;
        pQue->pTail = pNew;
    }

    pQue->size++;

    return 0;
}

int queGet(pQue_t pQue, pNode_t *pGet)
{
    if(0 == pQue->size){
        return -1;
    }

    *pGet = pQue->pHead;
    pQue->pHead = pQue->pHead->pNext;

    pQue->size--;

    if(0 == pQue->size){
        pQue->pTail = NULL;   
    }

    return 0;
}

