~~~c
通过tcp协议实现一个聊天室，多个客户端登录服务器，登录后一个客户端发送的消息，服务器转发给其他客户端。
~~~



### 服务器

~~~c
#include<func.h>                                                              
int main(int argc,char **argv){
      int ret=0;
      int sfd=socket(AF_INET,SOCK_STREAM,0);
      ERROR_CHECK(sfd,-1,"socket");
  
      struct sockaddr_in myaddr;
      memset(&myaddr,0,sizeof(myaddr));
      myaddr.sin_family=AF_INET;
      myaddr.sin_addr.s_addr=inet_addr(argv[1]);
      myaddr.sin_port=htons(atoi(argv[2]));
  
      socklen_t reuse=1;
      setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
  
      ret=bind(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
      ERROR_CHECK(ret,-1,"bind");
  
      ret=listen(sfd,10);
      ERROR_CHECK(ret,-1,"listen");
  
      fd_set rdset;
      fd_set needset;
      FD_ZERO(&rdset);
      FD_ZERO(&needset);
  
      FD_SET(sfd,&needset);
      char buf[64]={0};
      
      struct sockaddr_in addr;
      memset(&addr,0,sizeof(addr));
      socklen_t len=sizeof(addr);
  
      int rdynum=0;
      int newfd[2]={0};//accept生成两个客户端的文件描述符，用于服务器和它们两个的通信
      while(1){
          memcpy(&rdset,&needset,sizeof(fd_set));
          rdynum=select(10,&rdset,NULL,NULL,0);
          for(int i=0;i<rdynum;i++){
              //当第二个客户端处于就绪状态
              if(FD_ISSET(newfd[1],&rdset)){
                  memset(buf,0,sizeof(buf));
                  ret=recv(newfd[1],buf,sizeof(buf),0);
  
                  if(0==ret){
                      printf("886\n");
                      close(newfd[1]);
                      FD_CLR(newfd[1],&needset);
                      continue;
                  }   
                  send(newfd[0],buf,strlen(buf),0);
              }   
              //第一个客户端处于就绪状态
              if(FD_ISSET(newfd[0],&rdset)){
                  memset(buf,0,sizeof(buf));
                  ret=recv(newfd[0],buf,sizeof(buf),0);
  
                  if(0==ret){
                      printf("see you\n");
                      close(newfd[0]);
                      FD_CLR(newfd[0],&needset);
                      continue;
                  }   
                  send(newfd[1],buf,strlen(buf),0);
              }   
  			 //sfd处于就绪状态
              if(FD_ISSET(sfd,&rdset)){
                  //生成第一个客户端的文件描述符
                 newfd[0]=accept(sfd,(struct sockaddr*)&addr,&len);
                 ERROR_CHECK(newfd[0],-1,"accept");
                  printf("client1 ip=%s\n",inet_ntoa(addr.sin_addr));
                  printf("client1 port=%d\n",ntohs(addr.sin_port));
                  FD_SET(newfd[0],&needset);
                  printf("client1 already connect!\n");
  				//生成第二个客户端的文件描述符
                  newfd[1]=accept(sfd,(struct sockaddr*)&addr,&len);
                 ERROR_CHECK(newfd[1],-1,"accept");
                  printf("client2 ip=%s\n",inet_ntoa(addr.sin_addr));
                  printf("client2 port=%d\n",ntohs(addr.sin_port));
                  FD_SET(newfd[1],&needset);
                  printf("client2 already connect!\n");
              }   
          }   
      }   
      close(sfd);
      close(newfd[0]);
      close(newfd[1]);
      return 0;
  }

//小bug:客户端断开以后再重连，发的消息其他客户端就收不到了
~~~

### 客户端

~~~c
#include <func.h>

int main(int argc, char **argv){
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(&myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = inet_addr(argv[1]);
    myaddr.sin_port = htons(atoi(argv[2]));
    
    connect(sfd, (struct sockaddr*)&myaddr, sizeof(myaddr));

    fd_set rdset;

    FD_ZERO(&rdset);

    char buf[64] = {0};

    int rdynum = 0;

    while(1){
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(sfd, &rdset);

        rdynum = select(sfd + 1, &rdset, NULL,NULL, 0);
        for(int i = 0; i < rdynum; ++i){
            if(FD_ISSET(STDIN_FILENO, &rdset)){
                memset(buf, 0, sizeof(buf));
                read(STDIN_FILENO, buf, sizeof(buf));
                send(sfd, buf, strlen(buf) - 1, 0);
            }

            if(FD_ISSET(sfd, &rdset)){
                memset(buf, 0, sizeof(buf));
               int ret = recv(sfd, buf, sizeof(buf) - 1, 0);
               if(0 == ret){ //服务器断开连接，客户端肯定要断啦
                   printf("88htons\n");
                   close(sfd);
                   FD_CLR(sfd, &rdset);
                   return 0;
               }

               if(0 == strcmp(buf, "")){
                   printf("please\n");
                   return 0;
               }
               printf("buf = %s\n", buf);
            }
        }
    }
    close(sfd);
    return 0;
}

~~~

### 结果如下

~~~c
//服务器
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./tcpduo 192.168.196.128 2000
client1 ip=192.168.196.128
client1 port=58270
client1 already connect!
client2 ip=192.168.196.128
client2 port=58272
client2 already connect!
^C
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ 
//客户端1
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./tcpclient 192.168.196.128 2000
hello
buf = nihao
imissyou
buf = me too
88htons
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ 
//客户端2
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ ./tcpclient 192.168.196.128 2000
buf = hello
nihao
buf = imissyou
me too
88htons
[wuqingtong@wuqingtong-virtual-machine: ] ~/Linux $ 
~~~



