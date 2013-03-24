%{
  #include "def.h"
  int yylex ();
  void yyerror(char *);
  extern char *yytext;
%}

%token STRING

%%

line:
  command '\n'
  |'\n'
  |
  ;

command:
  fgCommand  { printf("a command\n"); return 0; }
  |fgCommand '&' { printf("a background command\n"); }
  ;

fgCommand:
  progInvocation redirect;

progInvocation:
  STRING args;

redirect:
  |'<' STRING redirect  { printf("input redirection\n"); }
  |'>' STRING redirect  { printf("output redirection\n"); }
  ;

args    :
  |args STRING;


%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}


int main(void) {
    while (1) {
        yyparse();
    }
    return 0;
}