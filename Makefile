CC=gcc
INC=$(shell pkg-config --cflags glib-2.0)
LIBS=$(shell pkg-config --libs glib-2.0)

%: %.c
	${CC} -std=c99 -o $@ $< -lncurses

calc_parser: calc_parser.c
	${CC} ${INC} -g -std=c99 -o $@ $< ${LIBS}

macros: macros.c
	${CC} -std=c99 -E $< > $@

flexThreads: flexThreads.cpp
	g++ -Wall -o $@ $^ -pthread

threadCleanup: threadCleanupHandler.cpp
	g++ -Wall -o $@ $^ -pthread

clean:
	@-rm -rf flexThreads
	@-rm -rf threadCleanup
	@-rm -rf hello_ncurses macros calc_parser
