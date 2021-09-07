#include<func.h>
//int recvcycle(int sockFd,void * buf,int totallen);
int main(int argc,char **argv){

    int ret=0;
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");

    struct sockaddr_in myaddr;
    memset(&myaddr,0,sizeof(myaddr));
    myaddr.sin_family=AF_INET;
    myaddr.sin_addr.s_addr=inet_addr(argv[1]);
    myaddr.sin_port=htons(atoi(argv[2]));

    ret=connect(sfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
    ERROR_CHECK(ret,-1,"connect");
    
    int datalen=0;
    char buf[1000]={0};
    //接收火车头
    recv(sfd,&datalen,sizeof(int),0);
    //接收火车车厢
    recv(sfd,buf,datalen,0);

    //创建同名文件
    int fd;
    fd=open(buf,O_RDWR|O_CREAT,0666);//可读可写给权限
    ERROR_CHECK(fd,-1,"open");

    off_t filesize=0;
    off_t recvlen=0;

    recv(sfd,&datalen,4,0);
    recv(sfd,&filesize,datalen,0);
    printf("filesize=%ld\n",filesize);

    float rate=0;
    int num=0;
    while(1){
        memset(buf,0,sizeof(buf));
        //接收文件内容长度
        recv(sfd,&datalen,sizeof(int),0);

        if(1000!=datalen){
        if(0==datalen){
            break;
        }
            printf("datalen=%d\n",datalen);
        }

       // ret=recvcycle(sfd,buf,datalen);
        ret=recv(sfd,buf,datalen,MSG_WAITALL);
        //当前接收总字节
        num++;
        recvlen+=ret;
        rate=(float)recvlen/filesize*100;
        if(0==num%1000){
            printf("rate=%5.2f\r",rate);//回到行首
             fflush(stdout);
             

        }

            if(rate==100.00){
                printf("rate=%5.2f\n",rate);
            }

        //buf内容写到文件里
        write(fd,buf,ret);
    }
     printf("\n");
    return 0;
}
