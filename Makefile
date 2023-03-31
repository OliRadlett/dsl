parser:
	bison -d -o parser.c parser.y
	flex -o tokens.c parser.l
	gcc -g -o parser parser.c tokens.c main.c


clean:
	rm -f tokens.c
	rm -f parser.c
	rm -f parser.h
	rm -f parser