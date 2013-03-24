%{
	#include "y.tab.h"
%}

%%

[&<>\n]			{ return *yytext; }

[^ &<>\n\t\r]*	{ 
		// remove the commention for debugging
		// printf("a string: %s\n", yytext);
	  	return STRING; }

[ \t\r] 	;

%%

int yywrap(void) {
	return 1;
}

