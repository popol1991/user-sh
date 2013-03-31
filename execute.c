#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "def.h"
#include "internalcmd.h"
#include "cmd_list.h"

#define IS_INTERNAL(input, cmd) strcmp(input, cmd) == 0

char* _INT_CMD[6] = {"exit", "cd", "jobs", "history", "bg", "fg"};

int STDIO[2];
int is_bg = 0;
int count, len;
int pre_pipe[2], current_pipe[2];
cmd_list head;

void error_handler(int err) {
	switch (err) {
		case ENOENT:	printf("user-sh: command or filename not found\n");
				break;
		case EACCES:	printf("user-sh: permission denied\n");
				break;
		default:	printf("user-sh: error: %d\n", err);
    }
}

void redirect2(int file_desc, char* file, int is_append) {
    int f;
    if (file_desc == 0) {
		f = open(file, O_CREAT | O_RDONLY, S_IRWXU);
    } else {
		f = open(file, O_CREAT | O_WRONLY | (is_append?O_APPEND:O_TRUNC), 0644);
    }
    close(file_desc);
    dup(f);
}

void connect_pipe() {
	if (count != 1) {
		//printf("connect input pipe %d\n", pre_pipe[0]);
		close(0);
		dup2(pre_pipe[0], 0);
		close(pre_pipe[0]);
		close(pre_pipe[1]);
	}

	if (len != 1 && count != len) {
		//printf("connect output pipe %d\n", current_pipe[1]);
		close(1);
		dup2(current_pipe[1], 1);
		close(current_pipe[1]);
		close(current_pipe[0]);
	}

}

void reset_fd(int file_desc) {
	dup2(STDIO[file_desc], file_desc);
	close(STDIO[file_desc]);
}


void exec_ext_command(command cmd) {
    pid_t pid;
	char* temp;
    
    pid = fork();
    if (pid < 0) {
		printf("FORK ERROR\n");
		exit(2);
    } else if (pid == 0) {
		// printf("%d\n", count);
		connect_pipe();

		// child process 
		if (execvp(cmd->args[0], cmd->args) < 0) {
			error_handler(errno);
		}

		exit(0);
    } else {
		// parent process
		if (!is_bg) 
			wait(NULL);
    }	
}


int is_int_cmd(char* path) {
	int i;
	for (i = 0; i < LENGTH(_INT_CMD); i++) {
		if (strcmp(_INT_CMD[i], path) == 0) 
			return 1;
	}
	return 0;	
}

void execute() {
	cmd_list p = head->next;
	command cmd = NULL;
	count = 0;
	len = length(head);
	STDIO[0] = dup(0);
	STDIO[1] = dup(1);
	// printf("length: %d\n", len);
	while (p) {
		count++;
		cmd = p->cmd;
		
		pre_pipe[0] = current_pipe[0];
		pre_pipe[1] = current_pipe[1];
		if (len != 1 && count != len) {
			if (pipe(current_pipe) != 0) 
				printf("error in making pipe\n");
			//printf("current pipe: %d %d\n", current_pipe[0], current_pipe[1]);
		}

		// io redirection
		if (cmd->input) 
			redirect2(0, cmd->input, 0);
		if (cmd->output) 
			redirect2(1, cmd->output, cmd->append);
		
		
		//printf("before cmd %d\n", count);

		char* path = cmd->args[0];
		if (is_int_cmd(path)) {
			connect_pipe();

			// internal commands
			if (IS_INTERNAL(path, "exit")) {
				exit(0);
			} else if (IS_INTERNAL(path, "cd")) {
				cd( cmd->args[1]?cmd->args[1]:"~" );
			} else if (IS_INTERNAL(path, "jobs")) {
				jobs();
			} else if (IS_INTERNAL(path, "history")) {
				history();
			} else if (IS_INTERNAL(path, "fg")) {
				fg(0);
			} else if (IS_INTERNAL(path, "bg")) {
				bg(0);
			} else {
				printf("error internal command\n");
			}
		} else {
			// external commands
			exec_ext_command( cmd );
		}

		// reset io file descriptor
		if (cmd->input)
			reset_fd(0);
		if (cmd->output) 
			reset_fd(1);

		if (len != 1 && count != len) 
			close(current_pipe[1]);

		//printf("after cmd %d\n", count);
		p = p->next;
	}
	clear_list(head);
} 

