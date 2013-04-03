#include <unistd.h>
#include <signal.h>

#include "def.h"
#include "cmd_list.h"

char *USER;
extern cmd_list head;
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

void prompt() {
    printf("%s@%s> ", USER, get_current_dir());
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


//int set_signal_handler() {
 //   if (signal(SIGINT, interupt_current) == SIG_ERR) {
//	fputs("An error occurred while setting an signal handler.\n", stderr);
//	return EXIT_FAILURE;
 //   }
//}

void init() {
    USER = getenv("USER");

    init_history();   
    // set_signal_handler();

    head = init_cmd_list();
}

