#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include "head.h"
#include "workque.h"

//线程池的数据结构
typedef struct{
    int threadNum;
    pthread_t *pthid;
    Que_t Que;
}ThreadPool_t, *pThreadPool_t;

typedef struct{
    int len;
    char buf[1000];
}Train_t;

int threadPoolInit(pThreadPool_t pPool, int threadNum);
int threadPoolStart(pThreadPool_t pPool);

int transFile(int clientFd);
int tcpInit(char *ip, char *port, int *sockFd);
int epollAddFd(int epfd, int fd);
#endif
