#include"processpool.h"

int sendFd(int pipeFd,int fd)
{
    struct msghdr msg;
    memset(&msg,0,sizeof(msg));

    struct iovec iov;
    memset(&iov,0,sizeof(iov));

    char buf[64]={0};
    strcpy(buf,"hello");

    iov.iov_base=buf;
    iov.iov_len=strlen(buf);

    msg.msg_iov=&iov;
    msg.msg_iovlen=1;

    size_t Len=CMSG_LEN(sizeof(int));
    struct cmsghdr *cmsg=(struct cmsghdr*)calloc(1,Len);

    cmsg->cmsg_len=Len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;

    msg.msg_control=cmsg;
    msg.msg_controllen=Len;

    *(int*)CMSG_DATA(cmsg)=fd;

    sendmsg(pipeFd,&msg,0);

    return 0;
}

