CC=gcc
INC=$(shell pkg-config --cflags glib-2.0)
LIBS=$(shell pkg-config --libs glib-2.0)

%: %.c
	${CC} -g -std=c99 -o $@ $< -lncurses -lm

qemu_test: qemu_test.c
	${CC} -D_POSIX_C_SOURCE=200112L ${INC} -g -std=c99 -o $@ $< ${LIBS}

calc_parser: calc_parser.c
	${CC} ${INC} -g -std=c99 -o $@ $< ${LIBS}

macros: macros.c
	${CC} -std=c99 -E $< > $@

flexThreads: flexThreads.cpp
	g++ -Wall -o $@ $^ -pthread

threadCleanup: threadCleanupHandler.cpp
	g++ -std=c++11 -Wall -o $@ $^ -pthread

shm-posix-producer-orig: shm-posix-producer-orig.c
	${CC} -g -o $@ $< -lrt

fifo: fifo_ping.c fifo_pong.c
	${CC} -g -std=c99 -o fifo_ping fifo_ping.c
	${CC} -g -std=c99 -o fifo_pong fifo_pong.c

tcp_file: tcp_file_client.c tcp_file_server.c
	${CC} -g -std=c99 -o tcp_file_client tcp_file_client.c
	${CC} -g -std=c99 -o tcp_file_server tcp_file_server.c

tcp_net: tcp_net_server.c tcp_net_client.c tcp_net_server2.c tcp_net_server3.c
	${CC} -g -std=c99 -o tcp_net_server tcp_net_server.c
	${CC} -g -std=c99 -o tcp_net_client tcp_net_client.c
	${CC} -g -std=c99 -o tcp_net_server2 tcp_net_server2.c
	${CC} -g -std=c99 -o tcp_net_server3 tcp_net_server3.c

ROOT=/opt/clang_llvm_3.9.0
CLANG=$(ROOT)/bin/clang++
FLAGS= -fcolor-diagnostics -std=c++14 -stdlib=libc++

misc: misc.cpp
	$(CLANG) $(FLAGS) $^ -o $@

threads: threads.cpp
	$(CLANG) $(FLAGS) $^ -lpthread -o $@

tidy:
	$(ROOT)/bin/clang-tidy

clean:
	@-rm -rf flexThreads calc_parser threadCleanup
	@-rm -rf hello_ncurses macros calc_parser qemu_test misc
	@-rm -rf tcp_net_server tcp_net_client tcp_net_server2

gdb:
	cgdb -dgdb -- -q

