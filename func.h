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

