#ifndef _def_H
#define _def_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYSTYPE char *

struct simple_cmd {
    int is_bg;
    int argc;
    int append;
    char *cmd;
    char **args;
    char *input;
    char *output;
};
typedef struct simple_cmd* command;

void init();
void prompt();
void execute();

#endif
