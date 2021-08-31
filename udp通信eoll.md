### udpepollserver.c

~~~c
#include <func.h>

void udpServer(){
    int ret = 0;
    //生成套接口,用于与对端建立连接时候使用
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr("192.168.196.128");
    myaddr.sin_port = htons(2800);

    ret = bind(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
    ERROR_CHECK(ret, -1, "bind");
    
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");
    struct epoll_event event,evs[2];
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    char buf[128] = {0};
    event.data.fd = sfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);

    ret = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &len);
    ERROR_CHECK(ret, -1, "recvfrom");
    printf("buf = %s\n", buf);

    int rdynum = 0;
    while(1){
        rdynum = epoll_wait(epfd, evs, 2, -1);
        for(int i = 0; i < rdynum; ++i){
            //如果标准输入就绪，就代表需要给对端发送数据
            if(evs[i].data.fd == STDIN_FILENO){
                memset(buf, 0, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                ret = sendto(sfd, buf, strlen(buf) - 1, 0,
                    (struct sockaddr*)&addr, len);
                ERROR_CHECK(ret, -1, "sendto");
            }
            //如果是newFd就绪，就代表对端有数据发送给我们，需要接收
            if(evs[i].data.fd == sfd){
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(sfd, buf, sizeof(buf) - 1, 0,
                    (struct sockaddr*)&addr, &len);
                ERROR_CHECK(ret, -1, "recvfrom");
                printf("buf = %s\n", buf);
            }
        }
    }
    close(sfd);
}
int main(){
    udpServer();
    return 0;
}
~~~

### udpepollclient.c

~~~c
#include <func.h>

void udpClient(){
    int ret = 0;
    //生成套接口,用于对对端建立连接时候使用
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr("192.168.196.128");
    myaddr.sin_port = htons(2800);
    
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    struct epoll_event event,evs[2];
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    event.data.fd = sfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
    socklen_t len = sizeof(myaddr);

    char buf[128] = {0};
    int rdynum = 0;
    while(1){
        rdynum = epoll_wait(epfd, evs, 2, -1);
        for(int i = 0; i < rdynum; ++i){
            if(evs[i].data.fd == STDIN_FILENO){
                memset(buf, 0, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                ret = sendto(sfd, buf, strlen(buf) - 1, 0,
                    (struct sockaddr*)&myaddr, len);
                ERROR_CHECK(ret, -1, "sendto");
            }
            if(evs[i].data.fd == sfd){
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(sfd, buf, sizeof(buf) - 1, 0,
                    (struct sockaddr*)&myaddr, &len);
                ERROR_CHECK(ret, -1, "recvfrom");
                printf("buf = %s\n", buf);
            }
        }
    }
    close(sfd);
 }
int main(){
    udpClient();
    return 0;
}
~~~

### 结果

~~~c
//服务端
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./udpserver 
buf = ll
tt
^C
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ 
//客户端
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./udpclient 
ll
buf = tt
^C
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ 
~~~

