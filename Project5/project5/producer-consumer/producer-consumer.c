#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "buffer.h"


int running = 0; // 0 -> running; 1 -> not running;

pthread_mutex_t mutex;
sem_t empty;
sem_t full;


void *producer(void *arg) {
    buffer_item item;

    while (1) {
        sleep(rand() % 5);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        if (running == 1) break;
        item = rand();
        if (insert_item(item) != -1) {
            fprintf(stdout, "[Producer] %d is produced.\n", item);
        } else {
            fprintf(stderr, "[Error] unreachable!\n");
			exit(1);
        }

        pthread_mutex_unlock(&mutex);
		sem_post(&full);
    }
    pthread_mutex_unlock(&mutex); // for terminating the program
	pthread_exit(0);
}

void *consumer(void *arg) {
    buffer_item item;
    
    while (1) {
        sleep(rand() % 5);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        if (running == 1) break;
        if (remove_item(&item) != -1) {
            fprintf(stdout, "[Consumer] %d is consumed.\n", item);
        } else {
            fprintf(stderr, "[Error] unreachable!\n");
			exit(1);
        }

        pthread_mutex_unlock(&mutex);
		sem_post(&empty);
    }
    pthread_mutex_unlock(&mutex); // for terminating the program
	pthread_exit(0);
}

int main(int argc, char *argv[]) {
    pthread_t *producer_thread, *consumer_thread;
    
    if (argc != 4) {
        fprintf(stderr, "[Error] Input should be like 'producer-consumer 10 5 5'.\n");
		exit(1);
    }

    int time_all = atoi(argv[1]);
    int producer_all = atoi(argv[2]);
    int consumer_all = atoi(argv[3]);

    
    initial_buffer();
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, max_buf);
    sem_init(&full, 0, 0);

    producer_thread = (pthread_t *) malloc (sizeof(pthread_t) * producer_all);
    consumer_thread = (pthread_t *) malloc (sizeof(pthread_t) * consumer_all);

    for (int i = 0; i < producer_all; i++)
		pthread_create(&producer_thread[i], NULL, &producer, NULL);
	for (int i = 0; i < consumer_all; i++)
		pthread_create(&consumer_thread[i], NULL, &consumer, NULL);

    sleep(time_all);

    running = 1;

    for (int i = 0; i < producer_all; i++)
		sem_post(&empty);
	for (int i = 0; i < consumer_all; i++)
		sem_post(&full);
    
    for (int i = 0; i < producer_all; i++)
		pthread_join(producer_thread[i], NULL);
	for (int i = 0; i < consumer_all; i++)
		pthread_join(consumer_thread[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    free(producer_thread);
    free(consumer_thread);
    return 0;
}