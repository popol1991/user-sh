# Lexer, flex under MacOS
LEX = lex
# yacc, bison under MacOS
YACC = bison -d -y
# C Compiler
CC = cc

# user-sh is the final excutable
user-sh: y.tab.o lex.yy.o execute.o utils.o cmd_list.o
	$(CC) -o user-sh y.tab.o lex.yy.o execute.o internalcmd.o utils.o cmd_list.o -ll -lm


lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: def.h cmd_list.o

utils.o: def.h cmd_list.o

execute.o: def.h internalcmd.o cmd_list.o

cmd_list.o: def.h

internalcmd.o: def.h

y.tab.c y.tab.h: yacc.y
	$(YACC) -v yacc.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

debug: CC += -g
debug: user-sh


clean:
	-rm -f *.o lex.yy.c *.tab.* user-sh *.output *~
