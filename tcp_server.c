#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#define SOCK_NM	"/tmp/lxc_tcp.sock"
#define LISTEN_BACKLOG 1
#define BUFF_SZ 128

static void err_exit(const char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

static int sock_bind(int sock_fd)
{
	struct sockaddr_un un;
	const size_t un_sz = sizeof(struct sockaddr_un);

	if (!access(SOCK_NM, F_OK))
		unlink(SOCK_NM);

	memset(&un, 0, un_sz);
	un.sun_family = AF_UNIX;
	strncpy(un.sun_path, SOCK_NM, sizeof(un.sun_path) - 1);

	return bind(sock_fd, (const struct sockaddr *) &un, un_sz);
}

int main(int argc, char *argv[])
{
	int entry_sock;
	int conn_sock;
	int ret;

	entry_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (entry_sock == -1) err_exit("[!] socket");

	ret = sock_bind(entry_sock);
	if (ret == -1) err_exit("[!] bind");

	chmod(SOCK_NM, 0666);

	ret = listen(entry_sock, LISTEN_BACKLOG);
	if (ret == -1) err_exit("[!] listen");

	for(;;) {
		/* TODO: args */
		conn_sock = accept(entry_sock, NULL, NULL);
		if (conn_sock == -1) err_exit("[!] accept");
		puts("New incoming connection");

		ret = 0;
		char buff[BUFF_SZ];
		for (;;) {
			memset(buff, 0, BUFF_SZ);
			ret = read(conn_sock, buff, BUFF_SZ-1);
			if (ret == -1) err_exit("[!] read");

			if (!strncmp(buff, "", BUFF_SZ))
				break;
			puts(buff);
		}
		puts("Connection closed");

		close(conn_sock);
	}
	close(entry_sock);
	unlink(SOCK_NM);

	return 0;
}
