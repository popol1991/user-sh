%{
    #include "def.h"
    #include "cmd_list.h"

    int yylex ();
    void yyerror(char *);
    extern char *yytext;
    extern int is_bg;
    extern cmd_list head;

    command cmd = NULL;
%}

%token STRING

%%

line:
  command '\n'	 	{   return 0; 	}
    /* discard the rest of current command when syntax error */
  |error '\n'		{   yyerrok;
			    return 1;	
			}
  |'\n'			{   return 1;	}
  | /* empty */
  ;

command:
  fgCommand		{ is_bg = 0; }
  |fgCommand '&'	{ is_bg = 1; }
  ;

fgCommand:
  fgCommand '|' singleCommand	{   
				    insert(head, create_node(cmd)); 
				    init_temp_cmd(); 
				}
  |singleCommand		{      
				    insert(head, create_node(cmd));
				    init_temp_cmd(); 
				}
  ;

singleCommand:
  progInvocation redirect

progInvocation:
  STRING args		{
			    // The first argument, by convention, should point to the file name
			    // the array of pointers must be terminated by a NULL pointer
			    if (!cmd->args) cmd->args = malloc(sizeof(char *));
			    cmd->args[0] = $1;
			    cmd->argc++;
			    cmd->args[cmd->argc] = NULL;
			}

redirect:   /* empty */
  |input STRING redirect	{ if (!cmd->input) cmd->input = $2;  }
  |output STRING redirect	{ if (!cmd->output) cmd->output = $2; }
  ;

input:  '<'		

output:
  '>'			{   cmd->append = 0;	}
  |'>''>'		{   cmd->append = 1;	}
  ;

args:	    /* empty */
  |args STRING		{
			    // ugly implementation with dynamic allocated array
			    // should have know the number of args in AST		
			    cmd->args = (char**) realloc(cmd->args, sizeof(char *) * (cmd->argc + 3));
			    cmd->args[1 + (cmd->argc)++] = $2;
			}
  ;



%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

void init_temp_cmd() {
    cmd = malloc(sizeof(struct simple_cmd));
    cmd->argc = 0;
    cmd->args = NULL;
    cmd->append = 0;
    cmd->input = NULL;
    cmd->output = NULL;
}

void stop() {
    printf("stop here\n");
}

int main(void) {
    init();
    init_temp_cmd();
    while (1) {
        prompt();
        if (yyparse() == 0) {
	    execute();
	}
    }
    return 0;
}
