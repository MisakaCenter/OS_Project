/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct
{
    void (*function)(void *p);
    void *data;
}task;

struct work_queue
{
    task work;
    struct work_queue *next;
};

// task
task task_to_do;

// the work queue
struct work_queue worktodo;
struct work_queue *head, *tail;
// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

//mutex
pthread_mutex_t thread_mut;

//sem
sem_t thread_sem;

//todo or not todo
int running;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    tail -> next = (struct work_queue *) malloc (sizeof (struct work_queue));
    if (tail -> next == NULL) {
        fprintf(stderr, "[Error] cannot malloc memory!\n");
		exit(1);
    }

    tail = tail -> next;
    tail -> work = t;

    return 0;
}

// remove a task from the queue
task dequeue() 
{
    if (head == tail) {
        fprintf(stderr, "[Error] No work remains!\n");
		exit(1);
    }

    head = head -> next;

    return head -> work;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    while (TRUE) {
        sem_wait(&thread_sem);

        if (running == 1) break;

        pthread_mutex_lock(&thread_mut);
        task_to_do = dequeue();
        pthread_mutex_unlock(&thread_mut);

        // execute the task
        execute(task_to_do.function, task_to_do.data);
    }

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task_to_do.function = somefunction;
    task_to_do.data = p;
    
    pthread_mutex_lock(&thread_mut);
    int result = enqueue(task_to_do);
    pthread_mutex_unlock(&thread_mut);

    if (result == 0) {
        sem_post(&thread_sem);
    }
    return result;
}

// initialize the thread pool
void pool_init(void)
{
    running = 0;
    head = (struct work_queue *) malloc (sizeof (struct work_queue));
    if (head == NULL) {
        fprintf(stderr, "[Error] cannot malloc memory!\n");
		exit(1);
    }
    tail = head;
    head->next = NULL;

    // create mutex
    if (pthread_mutex_init(&thread_mut, NULL)){
        fprintf(stderr, "[Error] cannot create mutex!\n");
		exit(1);
    }

    // create semaphore
    if (sem_init(&thread_sem, 0, 0)) {
        fprintf(stderr, "[Error] cannot create semaphore!\n");
		exit(1);
    }

    // create threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        if(pthread_create(&bee[i],NULL,worker,NULL)) {
            fprintf(stderr, "[Error] cannot create thread!\n");
			exit(1);
        }
    }

    fprintf(stdout, "[ThreadPool] Initialize successfully!\n");
}

// shutdown the thread pool
void pool_shutdown(void)
{
    running = 1;

    // set semaphore
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        sem_post(&thread_sem);
    }

    // join
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        if (pthread_join(bee[i],NULL)) {
            fprintf(stderr, "[Error] cannot join thread!\n");
			exit(1);
        }
    }

    // destroy
    if (pthread_mutex_destroy(&thread_mut) || sem_destroy(&thread_sem)) {
        fprintf(stderr, "[Error] cannot destroy semaphore or mutex!\n");
		exit(1);
    }

    fprintf(stdout, "[ThreadPool] Shutdown successfully!\n");
}
