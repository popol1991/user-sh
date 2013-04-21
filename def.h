#ifndef _def_H
#define _def_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#define YYSTYPE char *

#define LENGTH(x) sizeof(x)/sizeof((x)[0])

#define CMD_NUMBERS 100
#define MAXDIRLEN 100

#define RUNNING 1
#define STOPPED 0
#define DONE -1

struct simple_cmd {
    int argc;
    int append;
    char **args;
    char *input;
    char *output;
};
typedef struct simple_cmd* command;

struct command_list {
    command cmd;
    struct command_list* next;
};
typedef struct command_list* cmd_list;

struct ENV_HISTORY
{
    int start;
    int end;
    char *his_cmd[CMD_NUMBERS];
};
typedef struct ENV_HISTORY* Env_History;

struct pid_node {
    pid_t		pid;
    struct pid_node	*next;
};

struct jobs_node {
	struct pid_node* plist;
	char* cmd;
	int state;
	struct jobs_node* next;
};
typedef struct jobs_node * joblist;


void init();
void prompt();
void execute();
void init_temp_cmd();
void clear_temp_cmd(command cmd);
void stop();

#endif
