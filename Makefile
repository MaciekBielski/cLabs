CC=gcc

%: %.c
	${CC} -std=c99 -o $@ $< -lncurses

macros: macros.c
	${CC} -std=c99 -E $< > $@

flexThreads: flexThreads.cpp
	g++ -Wall -o $@ $^ -pthread

threadCleanup: threadCleanupHandler.cpp
	g++ -Wall -o $@ $^ -pthread

clean:
	@-rm -rf flexThreads
	@-rm -rf threadCleanup
	@-rm -rf hello_ncurses macros
