#include"processpool.h"

int recvFd(int pipeFd,int *fd)
{
    struct msghdr msg;
    memset(&msg,0,sizeof(msg));

    struct iovec iov;
    memset(&iov,0,sizeof(iov));

    char buf[8]={0};

    iov.iov_base=buf;
    iov.iov_len=sizeof(buf);

    msg.msg_iov=&iov;
    msg.msg_iovlen=1;

    size_t Len=CMSG_LEN(sizeof(int));
    struct cmsghdr *cmsg=(struct cmsghdr*)calloc(1,Len);

    cmsg->cmsg_len=Len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;

    msg.msg_control=cmsg;
    msg.msg_controllen=Len;

    recvmsg(pipeFd,&msg,0);
    *fd=*(int*)CMSG_DATA(cmsg);

    return 0;
}

