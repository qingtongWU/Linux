#include"processpool.h"

int makechild(pProcess_data_t pdata,int processnum){
    int fds[2];
    pid_t childpid;
    
    for(int i=0;i<processnum;++i){
        socketpair(AF_LOCAL,SOCK_STREAM,0,fds);

        childpid=fork();
        if(0==childpid){
            close(fds[1]);//子进程读管道，关闭写端
            childFunc(fds[0]);
            exit(0);
        }
        close(fds[0]);
        pdata[i].flag=0;
        pdata[i].pid=childpid;
        pdata[i].pipeFd=fds[1];
    }
    return 0;
}

int childFunc(int pipeFd){
    int clientFd=0;
    while(1){
        recvFd(pipeFd,&clientFd);
        transFile(clientFd);
        write(pipeFd,"world",5);
    }
    return 0;
}
