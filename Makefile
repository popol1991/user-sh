# Lexer, flex under MacOS
LEX = lex
# yacc, bison under MacOS
YACC = bison -d -y
# C Compiler
CC = cc

# user-sh is the final excutable
user-sh: y.tab.o lex.yy.o execute.o utils.o
	$(CC) -o user-sh y.tab.o lex.yy.o execute.o internalcmd.o utils.o -ll -lm


lex.yy.o: lex.yy.c y.tab.h
lex.yy.o y.tab.o: def.h

utils.o: def.h

execute.o: def.h internalcmd.o

internalcmd.o: def.h

y.tab.c y.tab.h: yacc.y
	$(YACC) -v yacc.y

lex.yy.c: lexer.l
	$(LEX) lexer.l

debug: y.tab.o lex.yy.o execute.o utils.o
	$(CC) -g -o user-sh y.tab.o lex.yy.o execute.o utils.o -ll -lm

clean:
	-rm -f *.o lex.yy.c *.tab.* user-sh *.output *~
