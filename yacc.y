%{
    #include "def.h"

    int yylex ();
    void yyerror(char *);
    extern char *yytext;
    extern command cmd;

    char *tmp;
    char **args = NULL;
%}

%token STRING

%%

line:
  command '\n'	 	{ 
			    args = NULL; 
			    return 0; 
			}
    /* discard the rest of current command when syntax error */
  |error '\n'		{   yyerrok;
			    return 1;	
			}
  |'\n'			{   return 1;	}
  | /* empty */
  ;

command:
  fgCommand		{ cmd->is_bg = 0; }
  |fgCommand '&'	{ cmd->is_bg = 1; }
  ;

fgCommand:
  progInvocation redirect

progInvocation:
  STRING args		{
			    cmd->cmd = $1;
			    // The first argument, by convention, should point to the file name
			    // the array of pointers must be terminated by a NULL pointer
			    if (!args) args = malloc(sizeof(char *));
			    args[0] = cmd->cmd;
			    args[cmd->argc] = NULL;
			    cmd->args = args;
			}

redirect:   /* empty */
  |input STRING redirect	{ if (!cmd->input) cmd->input = $2;  }
  |output STRING redirect	{ if (!cmd->output) cmd->output = $2; }
  ;

input:	
  '<'		
  |'<''<'   
  ;

output:
  '>'			{   cmd->append = 0;	}
  |'>''>'		{   cmd->append = 1;	}
  ;

args:	    /* empty */
  |args STRING		{
			    // ugly implementation with dynamic allocated array
			    // should have know the number of args in AST		
			    args = (char**) realloc(args, sizeof(char *) * (cmd->argc + 1));
			    args[(cmd->argc)++] = $2;
	    
			}
  ;




%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    init();

    while (1) {
        prompt();
        if (yyparse() == 0) {
	    execute();
	}
    }
    return 0;
}
