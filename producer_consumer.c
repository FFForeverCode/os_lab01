#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 20//定义消息队列中的最大消息数量

int buffer[MAX_BUFFER_SIZE];  // 消息队列
int count = 0;                // 当前消息队列中的消息数量

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//互斥锁
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;//消息量-当前是否未满
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;//消息量-当前是否不为空

// 生产者线程
void *producer(void *arg) {
    int item = 0;
    //循环生成
    while (1) {
        item++;
        //尝试获取互斥锁
        pthread_mutex_lock(&mutex);
        while (count == MAX_BUFFER_SIZE) {
            printf("消息队列已满，生产者等待...\n");
            pthread_cond_wait(&not_full, &mutex);
        }
        buffer[count++] = item;
        printf("生产者生产: %d，当前消息队列中消息的数量: %d\n", item, count);
        pthread_cond_signal(&not_empty);  // 唤醒消费者
        pthread_mutex_unlock(&mutex);//释放一下锁
        sleep(1);  //模拟生产耗时
    }
    return NULL;
}

// 消费者线程
void *consumer(void *arg) {
    int item;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            printf("消息队列为空，消费者等待...\n");
            pthread_cond_wait(&not_empty, &mutex);
        }
        item = buffer[--count];
        printf("消费者消费: %d，当前消息队列中的消息数量: %d\n", item, count);
        pthread_cond_signal(&not_full);  // 唤醒生产者
        pthread_mutex_unlock(&mutex);
        sleep(2);  // 模拟消费耗时
    }
    return NULL;
}

int main() {
    pthread_t prod_tid, cons_tid;
    pthread_create(&prod_tid, NULL, producer, NULL);
    pthread_create(&cons_tid, NULL, consumer, NULL);
    pthread_join(prod_tid, NULL);
    pthread_join(cons_tid, NULL);
    return 0;
}
