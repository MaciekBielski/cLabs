#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define _SOCK_ADDR "127.0.0.1"
#define _SOCK_PORT 50000
#define BUFF_SZ 128
#define MSG_NB 8

static void err_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static int sock_connect(int sock_fd)
{
	struct sockaddr_in in = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr(_SOCK_ADDR),
		.sin_port = htons(_SOCK_PORT),
		.sin_zero = {0},
	};
	const socklen_t in_sz = sizeof(struct sockaddr_in);

	return connect(sock_fd, (const struct sockaddr *) &in, in_sz);
}

int main(int argc, char *argv[])
{
	int conn_sock;
	int ret;
	const char *data[MSG_NB] = {
		"Aaa", "Bbb", "Ccc", "Ddd", "Eee", "Fff", "Ggg", "Hhh"
	};

	for(;;) {
		conn_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (conn_sock == -1) err_exit("[!] socket");

		ret = sock_connect(conn_sock);
		if (ret == -1) err_exit("[!] connect");

		for(size_t i=0; i<MSG_NB; i++) {
			ret = write(conn_sock, data[i], strlen(data[i])+1);
			if (ret == -1) err_exit("[!] write");
		}
		ret = write(conn_sock, "", 2);
		if (ret == -1) err_exit("[!] final write");

		/*! TODO: Close the connection here */
		shutdown(conn_sock, SHUT_RDWR);
		close(conn_sock);

		printf("Quit? [q]: ");
		char c = getchar();
		getchar();
		if (c=='q')
			break;
	}

	return 0;
}
