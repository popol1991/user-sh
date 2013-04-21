#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "def.h"
#include "internalcmd.h"
#include "cmd_list.h"

#define IS_INTERNAL(input, cmd) strcmp(input, cmd) == 0

extern Env_History envhis;

char* _INT_CMD[4] = {"cd", "jobs", "history", "bg"};

int STDIO[2];
int is_bg = 0;
int count, len;
int pre_pipe[2], current_pipe[2];
cmd_list head;
char* cmd_input;
extern joblist jlist;
int goon = 0;

struct pid_node* plist = NULL;

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

int is_int_cmd(char* path) {
    int i;
    for (i = 0; i < LENGTH(_INT_CMD); i++) {
	if (strcmp(_INT_CMD[i], path) == 0) 
	    return 1;
    }
    return 0;	
}

void exec_ext_command(command cmd) {
    if (execvp(cmd->args[0], cmd->args) < 0) {
	error_handler(errno);
    }
}

void exec_int_command(command cmd) {
    char* path = cmd->args[0];

    // internal commands
    if (IS_INTERNAL(path, "cd")) {
	cd( cmd->args[1]?cmd->args[1]:"~" );
    } else if (IS_INTERNAL(path, "jobs")) {
	jobs();
    } else if (IS_INTERNAL(path, "history")) {
	history();
    } else if (IS_INTERNAL(path, "fg")) {
        fg(atoi(cmd->args[1]));
    } else if (IS_INTERNAL(path, "bg")) {
        bg(atoi(cmd->args[1]));
    } else {
	printf("error internal command\n");
    }
}

void setGoon() {
    goon = 1;
}

void exec_command(command cmd) {
    struct pid_node *h;
    pid_t pid;
    char* temp;
    
    pid = fork();
    
    signal(SIGUSR1, setGoon);
    if (pid < 0) {
		printf("FORK ERROR\n");
		exit(2);
    } else if (pid == 0) {
        if (is_bg) {
            while (goon == 0);
            goon = 0;
        }

        connect_pipe();
	
		if (is_int_cmd( cmd->args[0] )) {
			exec_int_command(cmd);
		} else {
			exec_ext_command(cmd);
		}

		exit(0);
    } else {
		// parent process
		h = malloc(sizeof(struct pid_node));
		h->pid = pid;
		h->next = plist;
		plist = h;
    }	
}

void waitcld() {
    struct pid_node *p = plist;
    joblist j;
    int count = 0;
    if (is_bg) {
        add_job(plist, cmd_input);
        for (count = 0, j = jlist; j != NULL; j = j->next, count++);
        printf("[%d]\t%d\tRUNNING\t\t%s\n", count, plist->pid, cmd_input);
        for (p = plist; p != NULL; p = p->next)
            kill(p->pid, SIGUSR1);
    } else {
        for (p = plist; p != NULL; p = p->next) {
            int status;
            waitpid(p->pid, &status, WUNTRACED);
        }
    }
}

void execute() {
    cmd_list p = head->next;
    command cmd = NULL;
    count = 0;
    len = length(head);
    STDIO[0] = dup(0);
    STDIO[1] = dup(1);

    envhis->end = (envhis->end + 1) % CMD_NUMBERS;
    if (envhis->end == envhis->start) 
        envhis->start = (envhis->start + 1) % CMD_NUMBERS;
    envhis->his_cmd[envhis->end] = strdup(cmd_input);
    
    while (p) {
        count++;
        cmd = p->cmd;
        
        pre_pipe[0] = current_pipe[0];
        pre_pipe[1] = current_pipe[1];
        if (len != 1 && count != len) {
            if (pipe(current_pipe) != 0) 
                printf("error in making pipe\n");
        }

        // io redirection
        if (cmd->input) 
            redirect2(0, cmd->input, 0);
        if (cmd->output) 
            redirect2(1, cmd->output, cmd->append);
        
        if (strcmp(cmd->args[0], "exit") == 0) exit(0);
        
        if (IS_INTERNAL(cmd->args[0], "fg")) {
            fg(atoi(cmd->args[1]));
        } else if (IS_INTERNAL(cmd->args[0], "bg")) {
            bg(atoi(cmd->args[1]));
        } else if (IS_INTERNAL(cmd->args[0], "cd")) {
            cd(cmd->args[1]);
        } else {
            exec_command( cmd );
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
    waitcld();
    clear_plist();
    clear_list(head);
} 

