### tcpepollserver.c

~~~c
#include <func.h>

void tcpServer(){
    int ret = 0;
    //生成套接口,用于与对端建立连接时候使用
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr("192.168.196.128");
    myaddr.sin_port = htons(2200);
    //设置地址可重用
    socklen_t reuse = 1;//选项值设置为1，表示前面的宏生效
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    //绑定本机的ip和端口，绑定到sfd上
    ret = bind(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
    ERROR_CHECK(ret, -1, "bind");

    //监听对端的连接请求，并且能同时处理的最大连接数10
    ret = listen(sfd, 10);
    ERROR_CHECK(ret, -1, "listen");

    //accept返回时一个新的文件描述符，这个描述符用于后续与
    //客户端进行数据交互时使用的
    int newfd = 0;
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

    int rdynum = 0;
    while(1){
        rdynum = epoll_wait(epfd, evs, 2, -1);
        for(int i = 0; i < rdynum; ++i){
            //如果标准输入就绪，就代表需要给对端发送数据
            if(evs[i].data.fd == STDIN_FILENO){
                memset(buf, 0, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                send(newfd, buf, strlen(buf) - 1, 0);
            }
            //如果是newFd就绪，就代表对端有数据发送给我们，需要接收
            if(evs[i].data.fd == newfd){
                memset(buf, 0, sizeof(buf));
                ret = recv(newfd, buf, sizeof(buf) - 1, 0);
                 //如果recv返回值等于0，就说明对端连接断开了
                if(0 == ret){
                    printf("88\n");
                    close(newfd);
                    continue;//客户端断开后，下次还可以连接
                }
                printf("buf = %s\n", buf);
            }
            //代表对端连接请求
            if(evs[i].data.fd == sfd){
                newfd = accept(sfd, (struct sockaddr*)&addr, &len);
                ERROR_CHECK(newfd, -1, "accept");
                printf("client ip = %s\n", inet_ntoa(addr.sin_addr));
                printf("client port = %d\n", ntohs(addr.sin_port));
                
                event.events = EPOLLIN;
                event.data.fd = newfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
                printf("hello\n");
            }
        }
    }
    close(sfd);
    close(newfd);
}
int main(){
    tcpServer();
    return 0;
}
~~~

### tcpepollclient.c

~~~c
#include <func.h>

int tcpClient(){
    int ret = 0;
    //生成套接口,用于对对端建立连接时候使用
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sfd, -1, "socket");

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr("192.168.196.128");
    myaddr.sin_port = htons(2200);

    ret = connect(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
    ERROR_CHECK(ret, -1, "connect");
    
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    struct epoll_event event,evs[2];
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    event.data.fd = sfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);

    char buf[128] = {0};
    int rdynum = 0;
    while(1){
        rdynum = epoll_wait(epfd, evs, 2, -1);
        for(int i = 0; i < rdynum; ++i){
            if(evs[i].data.fd == STDIN_FILENO){
                memset(buf, 0, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                send(sfd, buf, strlen(buf) - 1, 0);
            }
            if(evs[i].data.fd == sfd){
                memset(buf, 0, sizeof(buf));
                ret = recv(sfd, buf, sizeof(buf) - 1, 0);
                if(0 == ret){
                    printf("see you\n");
                    close(sfd);
                    return 0;//服务器断开后，客户端自动退出
                }
                printf("buf = %s\n", buf);
            }
        }
    }
    close(sfd);
 }
int main(){
    tcpClient();
    return 0;
}
~~~

### 结果如下

~~~c
//服务端
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./tcpserver
client ip = 192.168.196.128
client port = 53902
hello
buf = haha
enen
buf = liue
88
client ip = 192.168.196.128
client port = 53906
hello
buf = youlaile
wozoula
^C
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $
//客户端
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./tcpclient
haha
buf = enen
liue

[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./tcpclient
youlaile
buf = wozoula
see you
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ 
~~~

