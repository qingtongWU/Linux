#include "../include/func.h"

int epollAddFd(int epfd, int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);

    return 0;
}
