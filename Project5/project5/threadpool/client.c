/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    int tsk_num = 30;
    // create some work to do
    struct data work[tsk_num];
    for (int i = 0; i < tsk_num; i++) {
        work[i].a = rand() % 50;
        work[i].b = rand() % 50;
    }

    // initialize the thread pool
    pool_init();
    for (int i = 0; i < tsk_num; i++) {
    // submit the work to the queue
        if(pool_submit(&add,&work[i])) {
            fprintf(stderr, "[Task %d] cannot submit to the pool.\n", i);
        }
    }
    // may be helpful 
    sleep(3);

    pool_shutdown();

    return 0;
}
