#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "def.h"
#include "internalcmd.h"
#include "cmd_list.h"

#define IS_INTERNAL(input, cmd) strcmp(input, cmd) == 0

int STDIO[2];
int is_bg = 0;
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
	STDIO[file_desc] = dup(file_desc);	
    if (file_desc == 0) {
		f = open(file, O_CREAT | O_RDONLY, S_IRWXU);
    } else {
		f = open(file, O_CREAT | O_WRONLY | (is_append?O_APPEND:O_TRUNC), 0644);
    }
    close(file_desc);
    dup(f);
}


void exec_ext_command(command cmd) {
    pid_t pid;
    
    pid = fork();
    if (pid < 0) {
		printf("FORK ERROR\n");
		exit(2);
    } else if (pid == 0) {
		// child process 
		if (execvp(cmd->args[0], cmd->args) < 0) {
			error_handler(errno);
		}
		exit(3);
    } else {
		// parent process
		if (!is_bg) 
			wait(NULL);
    }	
}

void reset_fd(int file_desc) {
	dup2(STDIO[file_desc], file_desc);
	close(STDIO[file_desc]);
}

void execute() {
	cmd_list p = head->next;
	command cmd = NULL;
	while (p) {
		cmd = p->cmd;

		// io redirection
		if (cmd->input) 
			redirect2(0, cmd->input, 0);
		if (cmd->output)
			redirect2(1, cmd->output, cmd->append);

		// internal commands
		char* path = cmd->args[0];
		if (IS_INTERNAL(path, "exit")) {
			exit(0);
		} else if (IS_INTERNAL(path, "cd")) {
			cd(NULL);
		} else if (IS_INTERNAL(path, "jobs")) {
			jobs();
		} else if (IS_INTERNAL(path, "history")) {
			history();
		} else if (IS_INTERNAL(path, "fg")) {
			fg(0);
		} else if (IS_INTERNAL(path, "bg")) {
			bg(0);
		} else {
			// external commands
			exec_ext_command( cmd );
		}

		// reset io file descriptor
		if (cmd->input)
			reset_fd(0);
		if (cmd->output)
			reset_fd(1);

		p = p->next;
	}
	clear_list(head);
} 

