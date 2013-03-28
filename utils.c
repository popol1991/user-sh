#include <unistd.h>

#include "def.h"

char *USER;
extern command cmd;

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
	
	cmd = malloc(sizeof(struct simple_cmd));
	cmd->cmd = NULL;
	cmd->input = NULL;
	cmd->output = NULL;
	cmd->args = NULL;
	cmd->argc = 1;
}

void debug() {
    char **p;
    int i;
    printf("The command is: %s\n", cmd->cmd);
    printf("It is %s a background command\n", cmd->is_bg?"\b":"NOT");
    p = cmd->args;
    if (p) {
	printf("the arguments are: ");
	for (i=0;i<cmd->argc;i++) printf("%s\t", p[i]);
	printf("\n");
    } else {
	printf("the command has no arguments\n");
    }
    if (cmd->input) {
	printf("input directed to: %s\n", cmd->input);
    }
    if (cmd->output) {
	printf("output directed to: %s\n", cmd->output);
    }
}
