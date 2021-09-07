### 基本概念

~~~c
使用一个机制把所有可能导致阻塞的资源(本质是文件)管理起来，将等待状态从等待某个资源改成等待整个集合。
//select的原理
//找fd_set结构体声明
$ gcc -E add.c -o add.i
$ grep -nE fd_set add.i
看第一个所在行数，比如在1195
$ vim +1195 add.i
//fd_set结构体
typedef struct{
    _fd_mask _fds_bits[1024/(8*(int)sizeof(_fd_mask))];//一个数组
}fd_set;
数组
    元素类型  _fa_mask
	大小     1024/(8*(sizeof(_fd_mask))
总空间大小1024/8bytes=1024位 			
    位图：每一位代表文件描述符，FD_ZERO将每一位置为0，FD_SET将文件描述符对应的那一位置1
执行select时，将fd_set拷贝到内核态，轮询0~nfds-1
    轮询缺点：						改进：epoll
            1.时间复杂度高O(n)    		1.红黑树，2.高并发量
            2.并发量优先					3.通知机制，数据来了才检查
                 

//介绍
#include<sys/select.h>
int select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,
           struct timeval *timeout)
    //所监听的文件描述符的最大值+1,写，读，异常和超时导致的阻塞
    //返回值是所有集合中已经就绪的文件描述符的总数
void FD_CLR(int fd,fd_set *set)
int FD_ISSET(int fd,fd_set *set)//遍历集合，检查是否处于就绪
int FD_SET(int fd,fd_set *set)//创建集合
void FD_ZERO(fd_set *set)//初始化集合
//流程
1.创建集合，定义一个fd_set类型的变量;
2.初始化集合，FD_ZERO(&set);
3.把待监听的文件描述符注册到集合中，FD_SET;
4.使用select,会使进程处于阻塞状态；
5.若监听的fd中，有至少一个处于就绪，select不在阻塞进程；
6.遍历所有的fd,使用fd_ISSET;
7.若就绪，就获取资源。

//select超时机制
struct timeval{
	long tv_sec;// 秒
    long tv_usec;// 微秒
};

struct timeval timeout;
timtout.tv_sec=2;
timeout.tv_usec=500000;
int sret=select(fdr+1,&rdset,NULL,NULL,&timeout);
if(sret==0){
    printf("time out!\n");
    continue;
}
~~~

### 即时通信的实现

~~~c
//ctrl+D  关闭聊天，这里只考虑写端先关闭
//集合管理管道的读端和标准输入
//写端
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,3);
    // ./chat1 1.pipe 2.pipe
    int fdw=open(argv[1],O_WRONLY);//打开1号管道的写端
    int fdr=open(argv[2],O_RDONLY);//打开2号管道的读端
    puts("chat1 established");
    
    char buf[512]={0};
    fd_set rdset;//管理读的集合
    //循环外表示每次循环都不会改变，循环里表示每次循环都会改变
    while(1){
  //根据select形参，知在selec中传入的实参每次都会改变，所以初始化等一系列操作放到循环中
        FD_ZERO(&rdset);//初始化读的集合
        FD_SET(STDIN_FILENO,&rdset);//监听stdin
        FD_SET(fdr,&rdset);//监听管道的读端
        struct timeval timeout;//超时变量为结构体类型
        timeout.tv_sec=60;//一分钟不回消息，判断超时，系统结束对话
        //select可能需要改动
        time_t tt;//定义时间类型的变量 
        //加入了超时判断，如果超时，返回值为0
       int sret=select(fdr+1,&rdset,NULL,NULL,&timeout);
        //             最大的文件描述符加1
       if(sret==0){ //时间到了，集合中还没有文件处于就绪，自动处于就绪
           printf("liule\n");
           break;
       }
        if(FD_ISSET(STDIN_FILENO,&rdset)){//检查stdin是否处于就绪
             puts("msg from stdin");
            memset(buf,0,sizeof(buf));//清空buf
            //buf中读到输入缓冲区
            int ret=read(STDIN_FILENO,buf,sizeof(buf));
            if(ret==0){
                printf("I quit!\n");
                write(fdw,"88",2);
                break;
            }
            time(&tt);
            strcat(buf,ctime(&tt));//ctime()返回当地时间的字符串
            write(fdw,buf,strlen(buf));
        }
        if(FD_ISSET(fdr,&rdset)){//检查管道是否处于就绪
            puts("msg from pipe");
            memset(buf,0,sizeof(buf));
            //read上一个人写时间的数组
            int sret=read(fdr,buf,sizeof(buf));//读到文件终止符的时候返回值是0
            if(sret==0){
                printf("chat is end!\n");
                break;
            }
            puts(buf);
        }
    }
    return 0;
}
//读端
#include<func.h>                                                   
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,3);
    int fdr=open(argv[1],O_RDONLY);
    int fdw=open(argv[2],O_WRONLY);
    puts("chat2 established");
    char buf[512]={0};
    fd_set rdset;
    while(1){
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(fdr,&rdset);
        struct timeval timeout;
        timeout.tv_sec=60;
        time_t tt;
        int sret=select(fdr+1,&rdset,NULL,NULL,&timeout);
        if(sret==0){
            printf("liule\n");
            break;
        }
        if(FD_ISSET(STDIN_FILENO,&rdset)){
            puts("msg from stdin");
            memset(buf,0,sizeof(buf));
            int ret=read(STDIN_FILENO,buf,sizeof(buf));
            if(ret==0){
                printf("I quit!\n");
                write(fdw,"88",2);
                break;
            }
            time(&tt);
            strcat(buf,ctime(&tt));
            write(fdw,buf,strlen(buf));
        }
        if(FD_ISSET(fdr,&rdset)){
            puts("msg from pipe");
            memset(buf,0,sizeof(buf));
            int sret=read(fdr,buf,sizeof(buf));
            if(sret==0){
                printf("chat is end!\n");
                break;
            }
            puts(buf);
        }
    }
    return 0;

~~~

### select写阻塞问题

~~~c
//原理
写入内核管道缓冲区---暂存区---读缓冲区
内核管道写缓冲区
    空的时候才就绪
    缓冲区满的时候，再写入会阻塞

//write.c
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,2);
    int fdw=open(argv[1],O_WRONLY);
    int cnt=0;
    while(1){
        printf("cnt=%d\n",cnt++);
        write(fdw,"I miss you!",11)};
	}
return 0;
}

//read.c 只打开不读，就会阻塞啦
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,2);
    int fdr=open(argv[1],O_RDONLY);
    while(1);//没有读操作
return 0;
}
//用select管理写阻塞
#include<func.h>
int main(int argc,char *argv[]){
    ARGS_CHECK(argc,2);
    int fdr=open(argv[1],O_RDWR);//以非阻塞的方式打开管道的一端
    int fdw=open(argv[1],O_RDWR);
    int count=0;
    fd_set rdset;
    fd_set wrset;//定义读写变量
    char buf[16384]={0};
    while(1){
        FD_ZERO(&rdset);
        FD_ZERO(&wrset);//初始化集合
        FD_SET(fdr,&rdset);
        FD_SET(fdw,&wrset);//待监听的文件描述符注册到集合
        select(fdw+1,&rdset,&wrset,NULL,NULL);//异常，时间都不要
        //读
        if(FD_ISSET(fdr,&rdset)){
           printf("count=%d,read\n",count++);
           read(fdr,buf,1024);
          // sleep(1);//进程阻塞一秒
           }   
        //写
           if(FD_ISSET(fdw,&wrset)){
              printf("count=%d,write\n",count++);
              write(fdw,buf,4096);//写缓冲区是读的4倍，>4096的时候会处于永久阻塞的状态
             // sleep(1);                                                 
            }   
     }   
    return 0;
}
~~~

