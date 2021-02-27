#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#define _SOCK_ADDR "127.0.0.1"
#define _SOCK_PORT 50000
#define LISTEN_BACKLOG 1
#define FDS_SZ 4
#define BUFF_SZ 4
#define MSG_END ''

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

static inline void array_init(int *arr, size_t sz, int val)
{
	for (size_t i=0; i<sz; i++)
		arr[i] = val;
}

typedef struct _fd_array {
	int elems[FDS_SZ];
	int sz;
	int max;
	bool fragmented;
} fd_array;

static void fd_array_init(fd_array *arr)
{
	array_init(arr->elems, FDS_SZ, -1);
	arr->sz = 0;
	arr->max = -1;
	arr->fragmented = false;
}

static int fd_array_append(fd_array *arr, int elem)
{
	if (arr->sz == FDS_SZ)
		return -1;

	arr->elems[arr->sz] = elem;
	arr->sz += 1;

	if (elem > arr->max)
		arr->max = elem;

	return 0;
}

/* Find holes marked as -1 and swap with last elements */
static int fd_array_defrag(fd_array *arr)
{
	int ret = 0;

	if (!arr->fragmented)
		return ret;

	for (size_t i=0; i<arr->sz; i++) {
		if (arr->elems[i] == -1) {
			ret++;
			arr->sz -= 1;
			arr->elems[i] = arr->elems[arr->sz];
			arr->elems[arr->sz] = -1;
		}
	}
	arr->fragmented = false;

	return ret;
}

/* Returns the number of merged elements */
static size_t fd_array_merge(fd_array *base, fd_array *tail)
{
	size_t i=0;

	while (i<tail->sz) {
		if (fd_array_append(base, tail->elems[i]))
			break;
		i++;
	}

	return i;
}

static void fd_set_fill(fd_set *set, const fd_array *arr)
{
	FD_ZERO(set);
    for (size_t i=0; i<arr->sz; i++) {
    	if (arr->elems[i] == -1)
    		continue;
		FD_SET(arr->elems[i], set);
    }
}

/* Returns #descriptors ready to read */
static int fd_set_select(fd_set *fdset, int fd_max)
{
	int ret;
    struct timeval tv = {
		.tv_sec = 10,
		.tv_usec = 0,
    };

	do {
		ret = select(fd_max+1, fdset, NULL, NULL, &tv);
	/* repeat if interrupted by signal */
	} while (ret == -1 && errno == EINTR);

    return ret;
}

static int accept_new_conn(int entry_sock)
{
	int ret;

	/* Try again if interrupted by a signal */
	do {
		ret = accept(entry_sock, NULL, NULL);
	} while (ret==-1 && errno==EINTR);
	if (ret == -1) err_exit("[!] accept scenario");
	return ret;
}

static bool is_end_of_message(const char *buff_pos)
{
	return (strchr(buff_pos, MSG_END) != NULL);
}

static void handle_not_accepted(fd_array *arr, int last_good)
{
    for (int i=last_good+1; i<arr->sz; i++)
    	close(arr->elems[i]);
}

static void buffer_flush(char * const buff, const char *buff_ptr, int *capacity)
{
	if (buff_ptr > buff)
		fprintf(stderr, "%s", buff);

	buff_ptr = buff;
	*capacity = BUFF_SZ;
	memset(buff, '\0', BUFF_SZ);
}

static int receive_data(int conn_sock)
{
	int max_space = BUFF_SZ;
	int sz;
	char buffer[BUFF_SZ];
	char *buff = buffer;

	memset(buff, '\0', BUFF_SZ);

	for (sz = 0;;) {
		sz = read(conn_sock, buff, max_space);
		if (sz == -1) {
			if (errno != EINTR)
				err_exit("[!] read");
			continue;
		}

		/* Connection closed */
		if (!sz)
			break;

		buff += sz;
		max_space -= sz;

		if (is_end_of_message(buff-sz))
			break;

		/* Buffer full */
		if (!max_space)
			break;
	}
	buffer_flush(buffer, buff, &max_space);

	return sz==0 ? -1 : 0;
}

static int dispatch_request(int conn_sock)
{
	return receive_data(conn_sock);
}

int main(int argc, char *argv[])
{
	int entry_sock;
	int ret;
	fd_array fds;
	fd_array_init(&fds);

	entry_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (entry_sock == -1) err_exit("[!] socket");

	/* local tests only */
    ret = 1;
	ret = setsockopt(entry_sock, SOL_SOCKET, SO_REUSEADDR, &ret, sizeof(int));
	if (ret) err_exit("[!] setsockopt");

	ret = sock_bind(entry_sock);
	if (ret == -1) err_exit("[!] bind");

	ret = listen(entry_sock, LISTEN_BACKLOG);
	if (ret == -1) err_exit("[!] listen");

	ret = fd_array_append(&fds, entry_sock);
	if (ret == -1) err_exit("[!] too many descriptors - array is full");

	for (;;) {
		/* re-initialize set from the list */
		fd_set rdset;
		fd_set_fill(&rdset, &fds);

		ret = fd_set_select(&rdset, fds.max);
		if (ret == -1) err_exit("[!] select failed");
		int ready_nb = ret;

		/* Find all ready descriptors to detect #ready_nb readable */
		int found = 0;
		fd_array fds_tmp;
		fd_array_init(&fds_tmp);

		for (size_t i=0; i<fds.sz && found<ready_nb; i++) {

			if (!FD_ISSET(fds.elems[i], &rdset))
				continue;
			else
				found++;

			if (fds.elems[i] == entry_sock) {
				/* New connection attempt received */
				ret = accept_new_conn(entry_sock);
				if (ret == -1) err_exit("[!] accept failed");
				fd_array_append(&fds_tmp, ret);
			} else {
				/* New request on existing connection */
				ret = dispatch_request(fds.elems[i]);
				/* Connection closed - mark for cleanup */
				if (ret == -1) {
					if (fds.max == fds.elems[i])
						fds.max = -1;
					fds.elems[i] = -1;
					fds.fragmented = true;
				}
			}
		}
		/* Remove connections marked for cleanup */
		fd_array_defrag(&fds);

		if (fds_tmp.sz) {
			/* Add new active connections */
			ret = fd_array_merge(&fds, &fds_tmp);
			if (ret < fds_tmp.sz)
				handle_not_accepted(&fds_tmp, ret);
		}
	}


	return 0;
}
