CC=gcc
INC=$(shell pkg-config --cflags glib-2.0)
LIBS=$(shell pkg-config --libs glib-2.0)

%: %.c
	${CC} -g -std=c99 -o $@ $< -lm

gdb:
	cgdb -dgdb -- -q

