### select缺点

~~~c
口诀： 加入集合找就绪，1024有修改，内核用户来回跑；
1. 每次调用将监听的文件描述符加入集合中，从用户空间拷贝到内核空间；
2. 每次都要遍历整个位图，找到就绪的文件描述符，每次都要将所用的文件描述符从内核拷贝到用户空间；
3. 位图有1024位，每次只能监听1024个，数量有限；
4. 每次调用都会修改集合中的数据
~~~

### epoll底层原理

~~~c
口诀： epoll -->红黑树-->双向链表-->用户空间结构体数组
1.epoll_ctl操作的是eventpoll结构体中的红黑树，就绪的文件描述符以键值对的方式加入红黑树中，内核给每个文件描述符注册一个回调函数；
2.当红黑树上的文件描述符就绪时，会主动调用自己的回调函数，加入到双向链表中；
3.epoll_wait从双向链表(表空则sleep())中取就绪的文件描述符，到用户空间的结构体数组中(根据业务并发量动态调整) 
~~~

### epoll优点

~~~c
口诀：红黑就绪有链表
1.把要监听的文件描述符加入红黑树(这个时候才有内存拷贝)，若不主动删除，则一直保持(一次注册，永久有效)；
2.文件描述符就绪的时候，会自己调用自己的回调函数，把自己加入双向链表中，不用遍历红黑树；
3.当双向链表中有数据的时候，epoll_wait从双向链表(表空则sleep())中取就绪的文件描述符，到用户空间的结构体数组中(根据业务并发量动态调整) 
    
// fd很少的时候,select可能比epoll效率高；
~~~

### epoll对文件描述符的操作

~~~c
// 联合体特点，配合结构体使用
1. LT(level triagger)水平触发，默认；
LT：当被监控的文件描述符上有可读写事件发生时，epoll_wait()会通知
处理程序去读写。如果这次没有把数据一次性全部读写完(如读写缓冲区太小)，那么下次调用epoll_wait()时，它还会通知你，在上次没读写完的文件描述符上继续读写.
   
2. ET(edge triagger)边缘触发
ET:当被监控的文件描述符上有可读写事件发生时，epoll_wait()会通知
处理程序去读写。如果这次没有把数据全部读写完(如读写缓冲区太小)，那么下次调用epoll_wait()时，它不会通知你，也就是它只会通知你一次，直到该文件描述符上出现第二次可读写事件才会通知你.

ET模式在很大程度上减少了epoll事件被重复触发的次数，因此效率要比LT模式高
ET模式一般配合非阻塞套接口使用
~~~

### 非阻塞接口

~~~c
1.recv()第四个参数设为 MSG_DONTWAIT，变为非阻塞的，没有接收数据会返回-1；
   if(-1 == ret){
       break;
   }

2.文件描述符属性的修改
void setNonBlock(int fd){
    int status = 0;
    //使用fcntl函数获取文件描述符的状态信息
    status = fcntl(fd, F_GETFL);
    //把文件描述符的状态设置为非阻塞的
    status |= O_NONBLOCK;
    //对描述符设置为非阻塞的
    fcntl(fd, F_SETFL, status);
}
// fcntl放在accept后面。

大多数阻塞函数，它的阻塞性和它的参数，也就是文件描述符有关，如果文件描述符数阻塞性的，则函数也是阻塞性的，否则反之。
~~~

### epoll_wait()的超时

~~~c
readyNum = epoll_wait(epfd, evs, 2, 5000);//单位ms，也就是5s
if(0 == readyNum){
	 close(newFd);//断开连接
}
~~~

### 设置缓冲区大小

~~~c
getsockopt() -- 获取缓冲区大小
setsockopt() -- 设置缓冲区大小

SOL_SOCKET
SO_RCVBUF --接收缓冲区大小(一般64KB)
SO_SNDBUF -- 发送缓冲区大小
SO_RECVLOWAT --接收缓冲区下限(大于下限的时候，打印数据，网络差的时候)
SO_SNDWAIT --发送缓冲区下限
    
//示例代码
//获取缓冲区大小
socklen_t bufSize = 0;
socklen_t len = sizeof(bufSize);
getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &bufSize, &len);
printf("bufSize = %d\n", bufSize);

//设置缓冲大小
bufSize = 4096;
setsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &bufSize, len);
getsockopt(sfd, SOL_SOCKET, SO_RCVBUF, &bufSize, &len);
printf("bufSize = %d\n", bufSize);

//设置接收缓冲下限
int lowat = 10; 
socklen_t len = sizeof(lowat);
setsockopt(newFd, SOL_SOCKET, SO_RCVLOWAT, &lowat, len);
~~~

