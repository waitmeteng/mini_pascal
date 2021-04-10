CC:=gcc
LEX:=flex
YACC:=yacc

mini_pascal: y.tab.o lex.yy.o node.o symtab.o codegene.o
	$(CC) -o mini_pascal y.tab.o lex.yy.o node.o symtab.o codegene.o -lfl -ly

lex.yy.o: lex.yy.c y.tab.h
node.o: node.c node.h
symtab.o: symtab.c symtab.h
codegene.o: codegene.c codegene.h
y.tab.c:
	$(YACC) -d mini_pascal.y
lex.yy.c:
	$(LEX) mini_pascal.l

.PHONY: clean
clean:
	rm -rf mini_pascal y.tab.h y.tab.c lex.yy.c *.o  
