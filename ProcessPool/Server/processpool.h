#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<pwd.h>
#include<grp.h>
#include<unistd.h>
#include<dirent.h>
#include<time.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<sys/time.h>
#include<sys/sem.h>
#include<signal.h>
#include<pthread.h>
#include<sys/fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#define ARGS_CHECK(argc,num)  {if(argc!=num)\
    {fprintf(stderr,"args error!\n");return -1;}}
#define ERROR_CHECK(ret,num,func){if(ret==num)\
    {perror(func);return -1;}}

#define THREAD_ERROR_CHECK(ret,funcName)\
    do{\
        if(ret!=0){\
            printf("%s : %s\n",funcName,strerror(ret));\
        }\
    }while(0)

//父进程管理子进程使用的数据结构
typedef struct{
    short flag;
    pid_t pid;
    int pipeFd;
}Process_data_t,*pProcess_data_t;
//解决tcp粘包问题的小火车
typedef struct{
    int len;
    char buf[1000];
}train_t;

int makechild(pProcess_data_t pdata,int processnum);
int childFunc(int pipeFd);
int tcpinit(char *ip,char *port,int *sockFd);
int epolladdFd(int epfd,int fd);
int sendFd(int pipeFd,int fd);
int recvFd(int pipeFd,int *fd);
int transFile(int clientFd);
