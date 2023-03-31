test:
	flex test.l
	gcc -o program lex.yy.c
	./program

new:
	bison -d test.y
	flex test.l
	gcc lex.yy.c test.tab.c
	./a.out

new_improved:
	bison -d -o parser.c test.y
	flex -o tokens.c test.l
	gcc -g -o parser parser.c tokens.c main.c


clean:
	rm -f a.out
	rm -f lex.yy.c
	rm -f program
	rm -f test.tab.c
	rm -f test.tab.h