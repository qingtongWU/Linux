#include"processpool.h"
int epolladdFd(int epfd,int fd){
    struct epoll_event event;
    memset(&event,0,sizeof(event));

    event.events=EPOLLIN;//监听读事件
    event.data.fd=fd;

    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);
    return 0;
}
