### 服务端

~~~c
#include<func.h>
int main(int argc,char **argv){
      int ret=0;
      //生成套接口描述符
      int sfd=socket(AF_INET,SOCK_DGRAM,0);               
      ERROR_CHECK(sfd,-1,"socket");

      struct sockaddr_in myaddr;
      memset(&myaddr,0,sizeof(myaddr));
      myaddr.sin_family=AF_INET;
      myaddr.sin_addr.s_addr=inet_addr(argv[1]);
      myaddr.sin_port=htons(atoi(argv[2]));
	 //绑定本机的ip和端口号到sfd上
      ret=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
      ERROR_CHECK(ret,-1,"bind");

      fd_set rdset;
      FD_ZERO(&rdset);

      char buf[128]={0};
      struct sockaddr_in addr;
      socklen_t len=sizeof(addr);
	 //接收客户端的连接请求
      ret=recvfrom(sfd,buf,sizeof(buf),0,(struct sockaddr*)&addr,&len);
      ERROR_CHECK(ret,-1,"recvfrom");
      printf("buf=%s\n",buf);
      int rdynum=0;
     
      while(1){
          FD_SET(STDIN_FILENO,&rdset);
          FD_SET(sfd,&rdset);
          rdynum=select(10,&rdset,NULL,NULL,0);
          
          for(int i=0;i<rdynum;i++){
			//如果STDIN_FILENO就绪，则发送数据
              if(FD_ISSET(STDIN_FILENO,&rdset)){
                  memset(buf,0,sizeof(buf));
                  read(STDIN_FILENO,buf,sizeof(buf));
                  //发送数据
                 ret= sendto(sfd,buf,strlen(buf)-1,0,(struct sockaddr*)&addr,len);
                 ERROR_CHECK(ret,-1,"sento");
              }
			//如果sfd就绪，就接收数据并打印出来
              if(FD_ISSET(sfd,&rdset)){
                  memset(buf,0,sizeof(buf));
                  ret=recvfrom(sfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&addr,&len);
                  ERROR_CHECK(ret,-1,"recvfrom");
                  printf("buf=%s\n",buf);
              }
          }
      }
      close(sfd);
      return 0;
  }
~~~

## 客户端

~~~c
#include<func.h>                                                           
int main(int argc,char **argv){

      int ret=0;
      int sfd=socket(AF_INET,SOCK_DGRAM,0);
      ERROR_CHECK(sfd,-1,"socket");

      struct sockaddr_in myaddr;
      memset(&myaddr,0,sizeof(myaddr));
      myaddr.sin_family=AF_INET;
      myaddr.sin_addr.s_addr=inet_addr(argv[1]);
      myaddr.sin_port=htons(atoi(argv[2]));

      socklen_t len=sizeof(myaddr);

      fd_set rdset;
      FD_ZERO(&rdset);

      char buf[128]={0};

      int rdynum=0;
      while(1){
          FD_SET(STDIN_FILENO,&rdset);
          FD_SET(sfd,&rdset);

          rdynum=select(10,&rdset,NULL,NULL,0);
          for(int i=0;i<rdynum;i++){

              if(FD_ISSET(STDIN_FILENO,&rdset)){
                  memset(buf,0,sizeof(buf));
                  read(STDIN_FILENO,buf,sizeof(buf));
                 ret= sendto(sfd,buf,strlen(buf)-1,0,(struct sockaddr*)&myaddr,len);
                 ERROR_CHECK(ret,-1,"sento");
              }

              if(FD_ISSET(sfd,&rdset)){
                  memset(buf,0,sizeof(buf));
                  ret=recvfrom(sfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&myaddr,&len);
                  ERROR_CHECK(ret,-1,"recvfrom");
                  printf("buf=%s\n",buf);
              }
          }
      }
      close(sfd);
      return 0;
  }

~~~

## func.h

~~~c
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<pwd.h>
#include<grp.h>
#include<unistd.h>
#include<dirent.h>
#include<time.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<sys/time.h>
#include<sys/sem.h>
#include<signal.h>
#include<pthread.h>

#define ARGS_CHECK(argc,num)  {if(argc!=num)\
    {fprintf(stderr,"args error!\n");return -1;}}

#define ERROR_CHECK(ret,num,func){if(ret==num)\
    {perror(func);return -1;}

#define THREAD_ERROR_CHECK(ret,funcName)\             
    do{\
        if(ret!=0){\
            printf("%s : %s\n",funcName,strerror(ret));\
        }\
    }while(0)

~~~

