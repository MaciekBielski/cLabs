CC=gcc
GLIB=-I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/glib-2.0

%: %.c
	${CC} -std=c99 -o $@ $< -lncurses

calc_parser: calc_parser.c
	${CC} ${GLIB} -std=c99 -o $@ $< -lncurses

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
