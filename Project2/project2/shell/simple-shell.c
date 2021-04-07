#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE		80 /* 80 chars per line, per command */
#define bool			int /* simulate boolean... */
#define true			1
#define false			0

void flatten(char *a, char *b) { // remove extra space, \n, \t
	int length = strlen(a);
	int current_location = 0;
	bool blank_end = false;
	for (int i = 0; i < length; i++) {
		if (a[i] == ' ' || a[i] == '\t' || a[i] == '\n') {
			if (blank_end) {
				b[current_location++] = ' ';
			}
			blank_end = false;
		} else {
			b[current_location++] = a[i];
			blank_end = true;
		}
	}
	if (b[current_location - 1] == ' ') {
		b[current_location - 1] = 0;
	}
	return;
}

int tokenize(char *a, char **b) {  //tokenize flatten string
	int num = 0; // token num
	int current_location = 0; // location inside string
	int length = strlen(a);
	if (length == 0) return 0;
	for(int i = 0; i < length; i++) {
		if (a[i] == ' ') {
			num++;
			current_location = 0;
		} else {
			b[num][current_location++] = a[i];
		}
	}
	return num + 1;
}

bool concurrentFlag(char *a) {
	int length = strlen(a);
	if (length == 0) return false;
	return (a[length - 1] == '&');
}

int main(void) {
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	char *raw_string;
	char *flatten_string;
	char *last_command;
    int should_run = 1;
	bool have_last_command = false;
	raw_string = (char*) malloc(MAX_LINE * sizeof(char));
	flatten_string = (char*) malloc(MAX_LINE * sizeof(char));
	last_command = (char*) malloc(MAX_LINE * sizeof(char));
    while (should_run){   
		if (concurrentFlag(flatten_string) == false) wait(NULL);
        printf("\033[42;37mosh>\033[2;7;0m"); // pretty print ~
        fflush(stdout);
		memset(raw_string, 0, sizeof(raw_string));
		memset(flatten_string, 0, sizeof(raw_string));
		fflush(stdout);
        fgets(raw_string, MAX_LINE, stdin);
		flatten(raw_string, flatten_string); // remove space \t \n from raw_string

		if (strcmp(flatten_string, "exit") == 0) {
			should_run = 0;
			break;
		}
		if (strcmp(flatten_string, "!!") == 0) {
			if (have_last_command) {
				strcpy(flatten_string, last_command);
				fprintf(stdout, "%s\n", flatten_string);
			} else {
				fprintf(stderr, "No commands in history.\n");
				continue;
			}
		} else {
			have_last_command = true;
			strcpy(last_command, flatten_string);
		}

		pid_t pid;
		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "Failed when creating a child process.\n");
			continue;
		}
		if (pid == 0) { // child
			for (int i = 0; i < MAX_LINE/2 + 1; i++) {
				args[i] = (char*) malloc(MAX_LINE * sizeof(char));
				memset(args[i], 0, sizeof(args[i]));
			}
			int command_num = tokenize(flatten_string, args);
			for (int i = command_num; i < MAX_LINE/2 + 1; i++) {
				free(args[i]);
				args[i] = NULL; 
			}
			
			if (concurrentFlag(flatten_string)) {
				free(args[command_num - 1]);
				args[command_num - 1] = NULL;
				command_num = command_num - 1;
			}

			int pipe_loc = -1;
			for (int i = 0; i < command_num; i++) {
				if (strcmp(args[i], "|") == 0) {
					pipe_loc = i;
					break;
				}
			}

			if (pipe_loc != -1) { // if pipe is need
				if (pipe_loc == 0 || pipe_loc == command_num -1) {
					fprintf(stderr, "Pipe command can't be conducted, expect more commands.\n");
				} else {
					int pipe_fd[2];
					int pipe_flag = pipe(pipe_fd);
					if (pipe_flag == -1) {
						fprintf(stderr, "Pipe can't be established.\n");
					} else { // pipe is created and no error, just do it ~
						pid = fork();
						if (pid < 0) {
							fprintf(stderr, "Failed when creating a child process.\n");
						}
						if (pid == 0) { // child of child
							for (int i = pipe_loc; i < command_num; i++) {
								free(args[i]);
								args[i] = NULL;
							}

							close(pipe_fd[0]); // close read
							if (dup2(pipe_fd[1], STDOUT_FILENO) < 0) {
								fprintf(stderr, "Dup failed.\n");
							} else {
								execvp(args[0], args);
							}
							close(pipe_fd[1]); // close write

							for (int i = 0; i < pipe_loc; i++) { // child of child menmory free
								free(args[i]);
							}
							free(raw_string);
							free(flatten_string);
							free(last_command);
							exit(0);
						} else { // child
							wait(NULL);

							for (int i = 0; i <= pipe_loc; i++) {
								free(args[i]);
							}
							for (int i = pipe_loc + 1; i < command_num; i++) { // move 2nd command to 1st
								args[i - pipe_loc - 1] = args[i];
							}
							for (int i = command_num - pipe_loc - 1; i < command_num; i++) {
								args[i] = NULL;
							}
							command_num = command_num - pipe_loc - 1;

							close(pipe_fd[1]);
							if (dup2(pipe_fd[0], STDIN_FILENO) < 0) {
								fprintf(stderr, "Pipe dup failed.\n");
							} else {
								execvp(args[0], args);
							}
							close(pipe_fd[0]);
						}
					}
				}
			} else { // no pipe, consider redirect input/output
				bool input_redirect_flag = false;
				bool output_redirect_flag = false;
				char *file_name;
				file_name = (char*) malloc(MAX_LINE * sizeof(char));
				if (command_num >= 3) {
					if ((strcmp(args[command_num - 2], "<") == 0) || (strcmp(args[command_num - 2], ">") == 0)) {
						if (strcmp(args[command_num - 2], "<") == 0) {
							input_redirect_flag = true;
						} else {
							output_redirect_flag = true;
						}
						strcpy(file_name, args[command_num - 1]);

						free(args[command_num - 1]);
						free(args[command_num - 2]);
						args[command_num - 1] = NULL;
						args[command_num - 2] = NULL;
						command_num = command_num - 2;
					}
				}
				int file_fd;
				if (input_redirect_flag == true) {
					file_fd = open(file_name, O_RDONLY, 0644);
					if (file_fd < 0) {
						fprintf(stderr, "File doesn't exist.\n");
					} else {
						if (dup2(file_fd, STDIN_FILENO) < 0) {
							fprintf(stderr, "Dup failed.\n");
						} else {
							execvp(args[0], args);
							close(file_fd);
						}
					}
				} else if (output_redirect_flag == true) {
					file_fd = open(file_name, O_WRONLY | O_CREAT, 0644);
					if (file_fd < 0) { 
						fprintf(stderr, "Can't create file.\n");
					} else {
						if (dup2(file_fd, STDOUT_FILENO) < 0) {
							fprintf(stderr, "Dup failed.\n");
						} else {
							execvp(args[0], args);
							close(file_fd);
						}
					}
				} else {
					execvp(args[0], args); // normal, no pipe, no redirect
				}
				free(file_name);
			}

			for (int i = 0; i < command_num; i++) { // child memory free
				free(args[i]);
			}
			free(raw_string);
			free(flatten_string);
			free(last_command);
			exit(0);
		} else { // father
			if (concurrentFlag(flatten_string) == false) wait(NULL);
		}
    }

	free(raw_string);
	free(flatten_string);
	free(last_command);
	return 0;
}
