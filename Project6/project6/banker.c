#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int check_safe();
int request_resources(int target, int req[]);
void release_resources(int target, int release[]);

int request_resources(int target, int req[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if (req[i] < 0) {
            fprintf(stdout, "[Error] Request less than 0! \n");
            return 1;
        }
        if (req[i] > need[target][i]) {
            fprintf(stdout, "[Error] Request more than need! \n");
            return 1;
        }
    }
            
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        available[i] -= req[i];
        allocation[target][i] += req[i];
        need[target][i] = maximum[target][i] - allocation[target][i];
    }

    if (check_safe() == 1) {
        // unsafe, rollback
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
            available[i] += req[i];
            allocation[target][i] -= req[i];
            need[target][i] = maximum[target][i] - allocation[target][i];
        }
        fprintf(stdout, "[Unsafe] Denied. \n");
        return 1;
    } else {
        fprintf(stdout, "[Safe] Accepted. \n");
        return 0;
    }
}

void release_resources(int target, int release[]){
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if (release[i] < 0) {
            fprintf(stdout, "[Error] Release less than 0! \n");
            return;
        }
        if (release[i] > allocation[target][i]) {
            fprintf(stdout, "[Error] Release more than allocated! \n");
            return;
        }
    }
    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
        available[i] += release[i];
        allocation[target][i] -= release[i];
        need[target][i] = maximum[target][i] - allocation[target][i];
    }
    fprintf(stdout, "[Safe] Accepted. \n");
    return;
}

int initialize(int argc, char *argv[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        available[i] = atoi(argv[i]);

    FILE *stream = fopen("max", "r");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        fscanf(stream, "%d", &maximum[i][0]);
        for (int j = 1; j < NUMBER_OF_RESOURCES; j++) {
            fscanf(stream, ",%d", &maximum[i][j]);
        }
    }
    fclose(stream);

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            need[i][j] = maximum[i][j];
            allocation[i][j] = 0;
        }
    return 0;
}

int check_safe() {
    int used[NUMBER_OF_CUSTOMERS];
    int available_[NUMBER_OF_RESOURCES];

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        available_[i] = available[i];

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
        used[i] = 0;
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        // find a customer to use
        int safe = 0;
        for (int j = 0; j < NUMBER_OF_CUSTOMERS; j++) {
            if (used[j] == 0) {
                int can = 1;
                for (int res = 0; res < NUMBER_OF_RESOURCES; res ++) {
                    if (need[j][res] > available_[res]) {
                        can = 0;
                        break;
                    }
                }
                if (can == 1) {
                    used[j] = 1;
                    safe = 1; // customer j can be satisfied!
                    for (int res = 0; res < NUMBER_OF_RESOURCES; res ++) {
                        available_[res] += allocation[j][res];
                    }
                    break;
                }
            }
        }
        if (safe == 0) {
            return 1;
        }
    }
    return 0;
}

void output() {
    fprintf(stdout, "#####     Current State     #####\n");
    fprintf(stdout, "  Available: [ ");
    for (int i = 0; i < NUMBER_OF_RESOURCES - 1; i++)
        fprintf(stdout, "%d , ", available[i]);
    fprintf(stdout, "%d ]\n", available[NUMBER_OF_RESOURCES - 1]);

    fprintf(stdout, "  Maximum: ");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (i != 0) fprintf(stdout, "           ");
        fprintf(stdout, "[ ");
        for (int j = 0; j < NUMBER_OF_RESOURCES - 1; j++) {
            fprintf(stdout, "%d , ", maximum[i][j]);
        }
        fprintf(stdout, "%d ]\n", maximum[i][NUMBER_OF_RESOURCES - 1]);
    }

    fprintf(stdout, "  Allocation: ");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (i != 0) fprintf(stdout, "              ");
        fprintf(stdout, "[ ");
        for (int j = 0; j < NUMBER_OF_RESOURCES - 1; j++) {
            fprintf(stdout, "%d , ", allocation[i][j]);
        }
        fprintf(stdout, "%d ]\n", allocation[i][NUMBER_OF_RESOURCES - 1]);
    }

    fprintf(stdout, "  Need: ");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (i != 0) fprintf(stdout, "        ");
        fprintf(stdout, "[ ");
        for (int j = 0; j < NUMBER_OF_RESOURCES - 1; j++) {
            fprintf(stdout, "%d , ", need[i][j]);
        }
        fprintf(stdout, "%d ]\n", need[i][NUMBER_OF_RESOURCES - 1]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != NUMBER_OF_RESOURCES + 1) {
        fprintf(stdout, "[Error] Wrong input. \n");
        exit(1);
    }
    initialize(argc, argv + 1);
    if (check_safe() == 1) {
        fprintf(stdout, "[Error] Unsafe initial state. \n");
        exit(1);
    }

    while (1) {
        char s1[100];
        fprintf(stdout, "Banker>>");
        fscanf(stdin, "%s", s1);
        if (strcmp(s1, "*") == 0) {
            output();
            continue;
        } else
        if (strcmp(s1, "RQ") == 0) {
            int target;
            fscanf(stdin, "%d", &target);

            int req[NUMBER_OF_RESOURCES];
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
                fscanf(stdin, "%d", &req[i]);
            }

            request_resources(target, req);
        } else
        if (strcmp(s1, "RL") == 0) {
            int target;
            fscanf(stdin, "%d", &target);

            int req[NUMBER_OF_RESOURCES];
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++){
                fscanf(stdin, "%d", &req[i]);
            }

            release_resources(target, req);
        } else {
            fprintf(stdout, "[Error] Unexpected input! \n");
        }
    }
    output();
}