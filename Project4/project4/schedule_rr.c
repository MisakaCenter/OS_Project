#include "schedulers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#include "cpu.h"
#include "task.h"

struct node *head = NULL; 

int time = 0;

void add(char *name, int priority, int burst) {
    Task *t = (Task *) malloc (sizeof(Task));

	t -> name = (char *) malloc (sizeof(char) * (1 + strlen(name)));
	strcpy(t -> name, name);
	t -> tid = __sync_fetch_and_add(&tid_value, 1);
	t -> priority = priority;
	t -> burst = burst;

	State *state = &(t -> state);
	state -> arrival = time;
	state -> last_execute = time;
	state -> waiting = 0;
	state -> response = 0;
	state -> turnaround = 0;

	insert(&head, t);
}

void schedule() {
	int task_num = 0;
	int turnaround_total = 0;
	int waiting_total = 0;
	int response_total = 0;

	while(head != NULL) {
		struct node *h = head;
		while (h -> next != NULL) {
			h = h -> next;
		}

		Task *task = h -> task;

		if (task -> burst <= QUANTUM) {
			run(task, task -> burst);
			delete(&head, task);

			State *state = &(task -> state);
			state -> waiting += time - state -> last_execute;
			if (state -> last_execute == state -> arrival) {
				state -> response = time - state -> last_execute;
			}
			state -> last_execute = time + task -> burst;
			state -> turnaround = time + task -> burst - state -> arrival;
		
			task_num ++;
			turnaround_total += state -> turnaround;
			waiting_total += state -> waiting;
			response_total += state -> response;

			time += task -> burst;

			free(task -> name);
			free(task);
		} else {
			run(task, QUANTUM);
			delete(&head, task);
			task -> burst = task -> burst - QUANTUM;

			State *state = &(task -> state);
			state -> waiting += time - state -> last_execute;
			if (state -> last_execute == state -> arrival) {
				state -> response = time - state -> last_execute;
			}
			state -> last_execute = time + QUANTUM;

			time += QUANTUM;
			insert(&head, task);
		}

		
	}
	printf("-----------------RESULT---------------------\n");
	printf("Total Time: %d , Total Task: %d \n", time, task_num);
	printf("Waiting Time: %d , Average: %.2lf \n", waiting_total, (double)waiting_total / task_num);
	printf("Turnaround Time: %d , Average: %.2lf \n", turnaround_total, (double)turnaround_total / task_num);
	printf("Response Time: %d , Average: %.2lf \n", response_total, (double)response_total / task_num);
	printf("-----------------RESULT---------------------\n");
}
