#include"processpool.h"

void sigFunc(int signalNum){
    printf("signal is comming\n");
}

int transFile(int clientFd){
    signal(SIGPIPE,sigFunc);

    //打开文件
    int fd=open("file",O_RDWR);
    ERROR_CHECK(fd,-1,"open");

    train_t train;
    memset(&train,0,sizeof(train));

    train.len=4;
    strcpy(train.buf,"file");

    //发送文件名
    send(clientFd,&train,train.len+4,0);//偏移len的int类型的4个字节数
    struct stat fileinfo;
    memset(&fileinfo,0,sizeof(fileinfo));

    fstat(fd,&fileinfo);

    train.len=sizeof(fileinfo.st_size);
    memcpy(train.buf,&fileinfo.st_size,train.len);
    
    send(clientFd,&train,train.len+4,0);
    printf("filesize=%ld\n",fileinfo.st_size);

    while(1){
        //读取文件内容到buf
        int ret1=read(fd,train.buf,sizeof(train.buf));
        train.len=ret1;

        int ret2=send(clientFd,&train,train.len+4,0);
        if(0==ret1){
            break;
        }

        if(-1==ret2){
            break;
        }
    }
    return 0;
}
