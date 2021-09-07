#include "../include/func.h"
#include "../include/workque.h"
#include "../include/threadPool.h"

int exitPipe[2];
void sigFunc1(int sigNum)
{
    printf("sig is comming\n");
    write(exitPipe[1], &sigNum, 4);
}

int main(int argc, char** argv)
{

    if(fork()){
        pipe(exitPipe);
        signal(SIGUSR1, sigFunc1);
        wait(NULL);

        exit(0);
    }

    int threadNum = atoi(argv[3]);

    ThreadPool_t pool;
    memset(&pool, 0, sizeof(pool));

    //1.初始化线程池
    threadPoolInit(&pool, threadNum);

    //2.启动线程池
    threadPoolStart(&pool);

    //3.创建tcp监听套接字
    int sfd = 0;
    tcpInit(argv[1], argv[2], &sfd);

    //4.创建epoll，把需要监听的文件描述符加入到集合
    int epfd = epoll_create(1);
    epollAddFd(epfd, sfd);
    epollAddFd(epfd, exitPipe[0]);

    struct epoll_event evs[2];
    memset(evs, 0, sizeof(evs));

    int newFd = 0;
    int readyNum = 0;

    while(1){
        readyNum = epoll_wait(epfd, evs, 2, -1);
        for(int i = 0; i < readyNum; ++i){
            if(evs[i].data.fd == sfd){
                newFd = accept(sfd, NULL, NULL);
                pNode_t pNode = (pNode_t)calloc(1, sizeof(Node_t));
                pNode->clientFd = newFd;

                pthread_mutex_lock(&pool.Que.mutex);
                queInsert(&pool.Que, pNode);
                pthread_cond_broadcast(&pool.Que.cond);
                pthread_mutex_unlock(&pool.Que.mutex);
            }
            if(evs[i].data.fd == exitPipe[0]){
                printf("pthread_cancel\n");
                for(int i = 0; i < threadNum; ++i){
                    pthread_cancel(pool.pthid[i]);
                }
                for(int i = 0; i < threadNum; ++i){
                    pthread_join(pool.pthid[i], NULL);
                }
                printf("child exit\n");

                exit(0);
            }
        }
    }

    return 0;
}

