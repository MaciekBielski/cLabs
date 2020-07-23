#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_NM	"/tmp/lxc_test.sock"
#define BUFF_SZ 128
#define MSG_NB 8

static void err_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static int sock_connect(int sock_fd)
{
	struct sockaddr_un un;
	const size_t un_sz = sizeof(struct sockaddr_un);

	memset(&un, 0, un_sz);
	un.sun_family = AF_UNIX;
	strncpy(un.sun_path, SOCK_NM, sizeof(un.sun_path) - 1);

	return connect(sock_fd, (const struct sockaddr *) &un, un_sz);
}

int main(int argc, char *argv[])
{
	int conn_sock;
	int ret;
	const char *data[MSG_NB] = {
		"Aaa", "Bbb", "Ccc", "Ddd", "Eee", "Fff", "Ggg", "Hhh"
	};

	for(;;) {
		conn_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
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
