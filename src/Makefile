LEX 	= lex
YACC 	= yacc -d -v
CC	= gcc -g -std=c99 -D_POSIX_SOURCE -Wall
LIBS	= -lfl


Shell: Shell.o analyse_expression.o umask.o commande.o y.tab.o lex.yy.o alias.o kill.o expression.o shell_system.o echo.o
	$(CC) -o Shell Shell.o analyse_expression.o alias.o echo.o kill.o expression.o umask.o commande.o shell_system.o y.tab.o lex.yy.o $(LIBS)

Shell.o: Shell.c Shell.h

alias.o: alias.c alias.h

analyse_expression.o: analyse_expression.c analyse_expression.h

kill.o: kill.c kill.h

echo.o: echo.c echo.h

umask.o: umask.c umask.h

commande.o: commande.c commande.h

expression.o: expression.c expression.h

shell_system.o: shell_system.c shell_system.h


lex.yy.o: lex.yy.c y.tab.h Shell.h

y.tab.c y.tab.h: Analyse.y
	$(YACC) Analyse.y

lex.yy.c: Analyse.l Shell.h y.tab.h
	$(LEX) Analyse.l


.PHONY: clean
clean:
	rm -f *.o y.tab.* y.output lex.yy.*
