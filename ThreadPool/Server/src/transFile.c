#include "../include/func.h"
#include "../include/threadPool.h"

void sigFunc(int sigNum)
{
    printf("sig is comming\n");
}

int transFile(int clientFd)
{
    
    signal(SIGPIPE, sigFunc);

    //1.打开文件
    int fd = open("file", O_RDWR);
    ERROR_CHECK(fd, -1, "open");
    
    Train_t train;
    memset(&train, 0, sizeof(train));

    train.len = 4;
    strcpy(train.buf, "file");

    //发送文件名
    send(clientFd, &train, 4 + train.len, 0);

    struct stat fileInfo;
    memset(&fileInfo, 0, sizeof(fileInfo));

    //发送文件的总长度
    fstat(fd, &fileInfo);

    train.len = sizeof(fileInfo.st_size);

    memcpy(train.buf, &fileInfo.st_size, train.len);

    send(clientFd, &train, 4 + train.len, 0);
    printf("fileSize = %ld\n", fileInfo.st_size);

    /* while(1){ */
    /*     //读文件内容到buf中 */
    /*     int ret = read(fd, train.buf, sizeof(train.buf)); */

    /*     train.len = ret; */

    /*     int ret1 = send(clientFd, &train, 4 + train.len, 0); */
    /*     if(0 == ret){ */
    /*         break; */
    /*     } */

    /*     //send一个已经断开的连接会返回-1 */
    /*     if(-1 == ret1){ */
    /*         break; */
    /*     } */
    /*     //发送文件内容 */

    /* } */

    //1.mmap实现零拷贝传输文件
    /* char *pMap = (char*)mmap(NULL, fileInfo.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); */
    /* ERROR_CHECK(pMap, (char*)-1, "pMap"); */

    /* send(clientFd, pMap, fileInfo.st_size, 0); */

    /* munmap(pMap, fileInfo.st_size); */

    //2.sendfile实现零拷贝
    /* sendfile(clientFd, fd, 0, fileInfo.st_size); */

    //3.splice实现零拷贝
    
    int fds[2];

    pipe(fds);

    int recvLen = 0;
    //pipe的是容量是65536个字节
    
    while(recvLen < fileInfo.st_size){
        int ret = splice(fd, 0, fds[1], 0, 128, 0);
        splice(fds[0], 0, clientFd, 0, ret, 0);
        recvLen += ret;
    }


    return 0;
}
