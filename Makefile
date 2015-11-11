CC=color-gcc

%: %.c
	${CC} -std=c99 -o $@ $< -lncurses

macros: macros.c
	${CC} -std=c99 -E $< > $@

clean:
	@-rm -rf hello_ncurses macros

