#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct memory {
    char *process;
    int space;
    struct memory * next;
};

struct memory *head;

void initialize(int space) {
    head = (struct memory *) malloc (sizeof(struct memory));
    head -> process = (char *) malloc (sizeof(char) * 20);
    strcpy(head -> process, "Unused");
    head -> space = space;
    head -> next = NULL;
    return;
}

void output() {
    struct memory *temp = head;
    int address = 0;
    while (temp != NULL) {
        fprintf(stdout, "Addresses [%d:%d] %s\n", address, address + temp -> space - 1, temp -> process);
        address += temp -> space;
        temp = temp -> next;
    }
    return;
}

void liner_compact() {
    struct memory *temp = head;
    while (temp -> next != NULL) {
        struct memory *n = temp -> next;
        if ((strcmp(temp -> process, "Unused") == 0) && (strcmp(n -> process, "Unused") == 0)) {
            temp -> space += n -> space;
            temp -> next = n -> next;
            free(n -> process);
            free(n);
        }else{
            temp = temp -> next;
        }
    }
}

void compact() {
    struct memory *temp = head -> next, *last = head;
    int free_sapce = 0;
    if (temp == NULL) return;
    if (strcmp(head -> process, "Unused") == 0) {
        struct memory *n = head;
        free_sapce += head -> space;
        head = head -> next;
        free(n -> process);
        free(n);
    }
    while (temp != NULL) {
        if (strcmp(temp -> process, "Unused") == 0) {
            free_sapce += temp -> space;
            last -> next = temp -> next;
            free(temp -> process);
            free(temp);
            temp = last -> next;
        } else {
            last = temp;
            temp = temp -> next;
        }
    }
    
    last -> next = (struct memory *) malloc (sizeof(struct memory));
    last = last -> next;
    last -> process = (char *) malloc (sizeof(char) * 20);
    strcpy(last -> process, "Unused");
    last -> space = free_sapce;
    last -> next = NULL;
    return;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
        fprintf(stdout, "[Error] Wrong input. \n");
        exit(1);
    }
    
    initialize(atoi(argv[1]));

    while (1) {
        char s1[100];
        fprintf(stdout, "allocator>>");
        fscanf(stdin, "%s", s1);
        if (strcmp(s1, "EXIT") == 0) {
            break;
        } else
        if (strcmp(s1, "STAT") == 0) {
            output();
        } else 
        if (strcmp(s1, "RQ") == 0) {
            char process_name[100], mode;
            int process_size;
            fscanf(stdin, "%s %d %c", process_name, &process_size, &mode);
            if (mode == 'F') {
                struct memory *temp = head;
                while (temp != NULL) {
                    if (strcmp(temp -> process, "Unused") == 0) {
                        if (temp -> space > process_size) {
                            struct memory *new = (struct memory *) malloc (sizeof(struct memory));
                            new -> process = (char *) malloc (sizeof(char) * 20);
                            strcpy(new -> process, "Unused");
                            new -> space = temp -> space - process_size;
                            new -> next = temp -> next;

                            strcpy(temp -> process, process_name);
                            temp -> space = process_size;
                            temp -> next = new;
                            
                            break;
                        } else if (temp -> space == process_size) {
                            strcpy(temp -> process, process_name);
                            temp -> space = process_size;

                            break;
                        }
                    }
                    temp = temp -> next;
                }
                if (temp == NULL) {
                    fprintf(stdout, "[Error] Out of Memory, you may need to compact!! \n");
                }
            } else if (mode == 'B') {
                struct memory *temp = head, *target = NULL;
                while (temp != NULL) {
                    if (strcmp(temp -> process, "Unused") == 0) {
                        if (temp -> space >= process_size) {
                            if (target == NULL) {
                                target = temp;
                            } else if (target -> space > temp -> space) {
                                target = temp;
                            }
                        }
                    }
                    temp = temp -> next;
                }
                if (target == NULL) {
                    fprintf(stdout, "[Error] Out of Memory, you may need to compact!! \n");
                } else {
                    if (target -> space > process_size) {
                        struct memory *new = (struct memory *) malloc (sizeof(struct memory));
                        new -> process = (char *) malloc (sizeof(char) * 20);
                        strcpy(new -> process, "Unused");
                        new -> space = target -> space - process_size;
                        new -> next = target -> next;

                        strcpy(target -> process, process_name);
                        target -> space = process_size;
                        target -> next = new;

                        } else if (target -> space == process_size) {
                            strcpy(target -> process, process_name);
                            target -> space = process_size;
                        }
                }
            } else if (mode == 'W') {
                struct memory *temp = head, *target = NULL;
                while (temp != NULL) {
                    if (strcmp(temp -> process, "Unused") == 0) {
                        if (temp -> space >= process_size) {
                            if (target == NULL) {
                                target = temp;
                            } else if (target -> space < temp -> space) {
                                target = temp;
                            }
                        }
                    }
                    temp = temp -> next;
                }
                if (target == NULL) {
                    fprintf(stdout, "[Error] Out of Memory, you may need to compact!! \n");
                } else {
                    if (target -> space > process_size) {
                        struct memory *new = (struct memory *) malloc (sizeof(struct memory));
                        new -> process = (char *) malloc (sizeof(char) * 20);
                        strcpy(new -> process, "Unused");
                        new -> space = target -> space - process_size;
                        new -> next = target -> next;

                        strcpy(target -> process, process_name);
                        target -> space = process_size;
                        target -> next = new;

                        } else if (target -> space == process_size) {
                            strcpy(target -> process, process_name);
                            target -> space = process_size;
                        }
                }
            } else {
                fprintf(stdout, "[Error] Unexpected mode! \n");
            }
        } else
        if (strcmp(s1, "RL") == 0) {
            char process_name[100];
            fscanf(stdin, "%s", process_name);

            struct memory *temp = head;
            while (temp != NULL) {
                if (strcmp(temp -> process, process_name) == 0) {
                    strcpy(temp -> process, "Unused");
                    liner_compact();
                    break;
                }
                temp = temp -> next;
            }
            if (temp == NULL) {
                fprintf(stdout, "[Error] Unexpected Process name! \n");
            }
        } else
        if (strcmp(s1, "C") == 0) {
            compact();
        } else {
            fprintf(stdout, "[Error] Unexpected input! \n");
        }
    }

    return 0;
}