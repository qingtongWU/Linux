#include "../include/head.h"

int tcpInit(char *ip, char *port, int *sockFd)
{
    int ret = 0; 

    //生成套接口,用于与对端建立连接时候使用
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));

    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(atoi(port));

    //设置地址可重用
    socklen_t reuse = 1; /* 选项值设置为1，表示前面的宏生效 */
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    //绑定本机的ip和端口，绑定到sfd上
    ret = bind(sfd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    ERROR_CHECK(ret, -1, "bind");

    //监听对端的连接请求，并且能同时处理的最大连接数10
    ret = listen(sfd, 10);
    ERROR_CHECK(ret, -1, "listen");

    //把sfd传出去
    *sockFd = sfd;

    return 0;
}
