~~~c
两个函数功能类似的情况下，参数越多的函数，功能越强大

time_t now;
time(&now);//获得现在的时间，用秒表示，1970年1月1日到现在
ctime(&now);//将现在的时间转换成字符串
由于ctime()是不安全函数，所以引入了ctime_r()；
    ctime_r(const time_t,char *buf)--- buf是线程的私有空间
//ctime的不安全性
#include <func.h>
void* threadFunc(void* p){
    time_t now;
    time(&now);
    char *pTime = ctime(&now);//数据存储在静态全局区会有数据覆盖
    printf("child pTime = %s\n", pTime);
    sleep(3);
    printf("child pTime = %s\n", pTime);

    pthread_exit(NULL);
}
int main(int argc, char** argv){
    pthread_t thid;
    int ret = 0;
    ret = pthread_create(&thid, NULL, threadFunc, NULL);
    sleep(2);
    time_t now;
    time(&now);
    char *pTime = ctime(&now);
    printf("main pTime = %s\n", pTime);
    pthread_join(thid, NULL);
    return 0;
}
结果发生了数据覆盖：
child pTime = Sat May 29 16:49:19 2021
main pTime = Sat May 29 16:49:21 2021
child pTime = Sat May 29 16:49:21 2021
//ctime_r()的安全性
#include <func.h>
void* threadFunc(void* p){
    time_t now;
    time(&now);
    char buf[64] = {0};//线程在自己独立的栈空间上申请内存
    ctime_r(&now, buf);
    printf("child pTime = %s\n", buf);
    sleep(3);
    printf("child pTime = %s\n", buf);
    pthread_exit(NULL);
}
int main(int argc, char** argv){
    pthread_t thid;
    int ret = 0;
    ret = pthread_create(&thid, NULL, threadFunc, NULL);
    sleep(2);
    time_t now;
    char buf[64] = {0};
    time(&now);
    ctime_r(&now, buf);
    printf("main pTime = %s\n", buf);
    pthread_join(thid, NULL);
    return 0;
}
结果没有发生数据覆盖：
child pTime = Sat May 29 16:54:09 2021
main pTime = Sat May 29 16:54:11 2021
child pTime = Sat May 29 16:54:09 2021

~~~

