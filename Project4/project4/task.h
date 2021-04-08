/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H

extern int tid_value;

typedef struct state {
    int arrival;
    int waiting;
    int response;
    int turnaround;
    int last_execute;
} State;

// representation of a task
typedef struct task {
    char *name;
    int tid;
    int priority;
    int burst;
    State state;
} Task;



#endif
