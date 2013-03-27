#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#include "def.h"
#include "internalcmd.h"

#define IS_INTERNAL(input, cmd) strcmp(input, cmd) == 0

command cmd;

void clear(command cmd) {
    // TODO: would introduce memory leak, need free pointers before set NULL
    cmd->argc = 1;
    cmd->cmd = NULL;
    cmd->input = NULL;
    cmd->output = NULL;
    cmd->args = NULL;
}


void exec_ext_command() {
    pid_t pid;
    
    pid = fork();
    if (pid < 0) {
	printf("FORK ERROR\n");
	exit(2);
    } else if (pid == 0) {
	if (execvp(cmd->cmd, cmd->args) < 0) {
	    switch (errno) {
		case ENOENT:	printf("user-sh: command or filename not found\n");
				break;
		case EACCES:	printf("user-sh: permission denied\n");
				break;
		default:	printf("user-sh: error\n");
	    }
	}
	exit(3);
    } else {
	wait(NULL);
	clear(cmd);
    }	
}

void execute() {
    // internal commands
    if (IS_INTERNAL(cmd->cmd, "exit")) {
	exit(0);
    } else if (IS_INTERNAL(cmd->cmd, "cd")) {
    	cd(NULL);
    } else if (IS_INTERNAL(cmd->cmd, "jobs")) {
    	jobs();
    } else if (IS_INTERNAL(cmd->cmd, "history")) {
    	history();
    } else if (IS_INTERNAL(cmd->cmd, "fg")) {
    	fg(0);
    } else if (IS_INTERNAL(cmd->cmd, "bg")) {
    	bg(0);
    } else {
	// external commands
	exec_ext_command();
    }
} 

