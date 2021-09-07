#include <func.h>

int recvcycle(int sockFd,void * buf,int totallen){
    int recvlen=0;
    int ret=0;
    while(recvlen<totallen){
        ret=recv(sockFd,(char *)buf+recvlen,totallen-recvlen,0);
        recvlen+=ret;
    }
    return recvlen;
}
