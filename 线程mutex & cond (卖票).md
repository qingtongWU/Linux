~~~c
/*
使用两个条件变量，消费者卖完票后通知生产者，然后在1号条件变量上面等待生产者放票，生产者放票后通知消费者，然后在2号条件变量上面等待消费者把票卖完，线程要写退出机制哦。
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
    int num;//票数
    int cir;//放票轮数
    pthread_mutex_t mutex;
    pthread_cond_t cond1;
    pthread_cond_t cond2;//有两个条件变量
}Data_t;

void* product(void* p)
{
    Data_t* p1 = (Data_t*)p;
    while(1){
        pthread_mutex_lock(&p1->mutex);
		//先判断有没有到放票轮数
        if(0== p1->cir){
            pthread_mutex_unlock(&p1->mutex);
            printf("set ticket over\n");
            break;//到了就跳出循环结束线程
        }
        else{
            pthread_cond_wait(&p1->cond1,&p1->mutex);//没到就先阻塞，等信号
            pthread_cond_broadcast(&p1->cond2);//告诉子线程有票了
            printf("set ticket begin!\n");
            p1->num=10000;//放票
            (p1->cir)--;
        }
        pthread_mutex_unlock(&p1->mutex);
    }
    pthread_exit(NULL);
}
void* sale1(void* p){
    Data_t* p1 = (Data_t*)p;
    int total=0;

    while (1){
        pthread_mutex_lock(&p1->mutex);//加锁
        if(0 != p1->num){//如果有票
            p1->num--;//消费者卖票，票数减少1
            total++;//消费者卖的票数加1
        }
        else{//没票时先判断有没有到放票轮数
            if(0==p1->cir){
                pthread_mutex_unlock(&p1->mutex);
                printf("Sale1 sale %d ticket!\n",total);
                break;
            }
            pthread_cond_signal(&p1->cond1);//先发信号要求放票
            pthread_cond_wait(&p1->cond2,&p1->mutex);//阻塞等待放票
        }
        pthread_mutex_unlock(&p1->mutex);//解锁
    }
    pthread_exit(NULL);
}

void* sale2(void* p)
{
    Data_t* p1 = (Data_t*)p;
    int total=0;

    while (1){
        pthread_mutex_lock(&p1->mutex);

        if(0 != p1->num){
            p1->num--;
            total++;
        }
        else{
            if(0==p1->cir){
                pthread_mutex_unlock(&p1->mutex);
                printf("Sale2 sale %d ticket!\n",total);
                break;
            }
            pthread_cond_signal(&p1->cond1);
            pthread_cond_wait(&p1->cond2,&p1->mutex);
        }
        pthread_mutex_unlock(&p1->mutex);
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t thid1;
    pthread_t thid2;
    pthread_t thid3;//线程类型定义
    Data_t data;
    memset(&data, 0, sizeof(data));
    data.num=100000;
    data.cir=6;

    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond1, NULL);
    pthread_cond_init(&data.cond2, NULL);

    pthread_create(&thid1, NULL, sale1, &data);
    pthread_create(&thid2, NULL, sale2, &data);
    pthread_create(&thid3, NULL, product, &data);

    pthread_join(thid1, NULL);
    pthread_join(thid2, NULL);
    pthread_join(thid3, NULL);

    pthread_mutex_destroy(&data.mutex);
    pthread_cond_destroy(&data.cond1);
    pthread_cond_destroy(&data.cond2);
    return 0;
}   

//结果如下
set ticket begin!
set ticket begin!
set ticket begin!
set ticket begin!
set ticket begin!
set ticket begin!
set ticket over
Sale1 sale 87821 ticket!
Sale2 sale 72179 ticket!
~~~

