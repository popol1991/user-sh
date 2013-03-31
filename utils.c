#define  _GNU_SOURCE
#include <unistd.h>

#include "def.h"
#include "cmd_list.h"

char *USER;
extern cmd_list head;

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
    printf("%s@%s> ", USER, get_current_dir_name());
}

void init() {
    USER = getenv("USER");

    head = init_cmd_list();
}

