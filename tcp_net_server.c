#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#define _SOCK_ADDR "10.0.3.1"
#define _SOCK_PORT 50000
#define LISTEN_BACKLOG 1
#define BUFF_SZ 4

static void err_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static int sock_bind(int sock_fd)
{
	struct sockaddr_in in = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr(_SOCK_ADDR),
		.sin_port = htons(_SOCK_PORT),
		.sin_zero = {0},
	};
	const socklen_t in_sz = sizeof(struct sockaddr_in);

	return bind(sock_fd, (const struct sockaddr *) &in, in_sz);
}

int main(int argc, char *argv[])
{
	int entry_sock;
	int conn_sock;
	int ret;

	entry_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (entry_sock == -1) err_exit("[!] socket");

	ret = sock_bind(entry_sock);
	if (ret == -1) err_exit("[!] bind");

	ret = listen(entry_sock, LISTEN_BACKLOG);
	if (ret == -1) err_exit("[!] listen");

	for(;;) {
		/* TODO: args */
		conn_sock = accept(entry_sock, NULL, NULL);
		if (conn_sock == -1) err_exit("[!] accept");
		puts("New incoming connection");

		char buff[BUFF_SZ];
		memset(buff, '\0', BUFF_SZ);

		ret = 0;
		for(;;) {
			for (int pos = 0; pos<BUFF_SZ; pos += ret) {
				ret = read(conn_sock, buff, BUFF_SZ);
				if (ret == -1) err_exit("[!] read");
				if (ret == 0) break;
			}

			if (!strncmp(buff, "", BUFF_SZ))
				break;
			puts(buff);
		}
		puts("Connection closed");

		close(conn_sock);
	}
	close(entry_sock);

	return 0;
}
