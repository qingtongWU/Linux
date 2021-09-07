#include"processpool.h"

int main(int argc,char **argv)
{
    int processnum = atoi(argv[3]);//子进程的数量
    //1.循环的创建子进程
   pProcess_data_t pdata=(pProcess_data_t)calloc(1,sizeof(Process_data_t));
   makechild(pdata,processnum);

   //2.创建tcp监听套接字
   int sfd=0;
   tcpinit(argv[1],argv[2],&sfd);

   //3.创建epoll,存储需要监听的文件描述符
   int epfd=epoll_create(2);
   ERROR_CHECK(epfd,-1,"create");
   epolladdFd(epfd,sfd);

   //循环的把父和所有子进程通信的管道加入监听集合中
   for(int i=0;i<processnum;++i){
       epolladdFd(epfd,pdata[i].pipeFd);
   }

   struct epoll_event evs[2];
   memset(&evs,0,sizeof(evs));

   int newfd=0;//新的tcp套接字
   int readnum=0;//就绪的文件描述符数量

   while(1){
       readnum=epoll_wait(epfd,evs,2,-1);
       //循环的确定就绪的文件描述符数量
       for(int i=0;i<readnum;++i){
           //如果sfd就绪，客户端建立连接
           if(evs[i].data.fd==sfd){
               newfd=accept(sfd,NULL,NULL);

               //循环的寻找空闲的子进程把newfd交给它
               for(int i=0;i<processnum;++i){
                   if(0==pdata[i].flag){
                       sendFd(pdata[i].pipeFd,newfd);

                       pdata[i].flag=1;//子进程置为忙碌
                       printf("childprocess %d is busy\n",pdata[i].pid);
                       break;
                   }
               }
               close(newfd);
           }
           //如果管道就绪
           else{
               for(int j=0;j<processnum;++j){
                   if(evs[i].data.fd==pdata[j].pipeFd){
                       //找到哪个子进程发来的消息
                       char buf[4]={0};
                       read(pdata[j].pipeFd,buf,4);

                       pdata[j].flag=0;
                       printf("childprocess %d is idle\n",pdata[i].pid);
                       break;
                   }
               }

           }
       }
   }
   return 0;
}
