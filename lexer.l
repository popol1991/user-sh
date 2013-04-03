/* it would be better to use substitution to make pattern more readible.
   it seems the substitution doesn't work on my machine, need help!	*/
META		[&<>\n]
continue	\\\n
%{
    #include "def.h"
    #include "y.tab.h"
%}

%%

	/* skip blank */
[ \t\r]	 	;

	/* the command is not finished yet when there's an '\' at the end of the line */
\\\n		; 


	/* metacharacter with special meaning in command */
[|&<>\n]		{ 	
			   //printf("meta: %s\n", yytext); 
			   return *yytext; 
			}

	/* string separated by space or metacharacter */
[^ \t\r|&<>\n]*		{ 
			    yylval = strdup(yytext);	// since yytext is part of the input buffer
							// and we don't wanna mingle with input buffer
							// we need to duplicate the string here
			    return STRING; 
			}

%%

int yywrap(void) {
	return 1;
}

