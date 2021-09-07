#include"processpool.h"
int tcpinit(char *ip,char *port,int *sockFd){

    int ret=0;
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");

    struct sockaddr_in myaddr;
    memset(&myaddr,0,sizeof(myaddr));

    myaddr.sin_family=AF_INET;
    myaddr.sin_addr.s_addr=inet_addr(ip);
    myaddr.sin_port=htons(atoi(port));
    //设置地址可重用
    socklen_t reuse=1;
    setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

    ret=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
    ERROR_CHECK(ret,-1,"bind");
    
    //监听对端的连接请求
    ret=listen(sfd,10);
    ERROR_CHECK(ret,-1,"listen");

    //把sfd传出去
    *sockFd=sfd;

    return 0;
}
