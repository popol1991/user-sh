#define  _GNU_SOURCE
#include <unistd.h>
#include <signal.h>

#include "def.h"
#include "cmd_list.h"
#include "internalcmd.h"

char *USER;
extern cmd_list head;
extern joblist jlist;
extern char* cmd_input;
extern Env_History envhis;

void clear_temp_cmd(command cmd) {
    int i;
    for (i=0; i<cmd->argc; i++) {
	free(cmd->args[i]);
    }
    cmd->argc = 0;
    cmd->args = NULL;
    cmd->append = 0;
    cmd->input = NULL;
    cmd->output = NULL;
}


char * get_current_dir() {
    char *pwd = malloc(256 * sizeof(char));
    getcwd(pwd, 256);
    while (strstr(pwd+1, "/") != 0x0) {
    	pwd = strstr(pwd+1, "/");
    }
    pwd++;
    return pwd;
}

void init_history() {
    int i;
    envhis = malloc(sizeof(struct ENV_HISTORY));
    envhis->start = 0;
    envhis->end = 0;
    for (i=0; i<CMD_NUMBERS; i++) {
    envhis->his_cmd[i] = NULL;
    }
}

void prompt() {
    printf("%s@%s> ", USER, get_current_dir());
}


void init() {
    USER = getenv("USER");
    jlist = NULL;        
    cmd_input = malloc(1024 * sizeof(char));    
    head = init_cmd_list();
    init_history();   
    
    signal(SIGTSTP, sigstp_handler);
    struct sigaction act;
    act.sa_sigaction = remove_job;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &act, NULL)) {
        printf("signal error\n");
    }
}

