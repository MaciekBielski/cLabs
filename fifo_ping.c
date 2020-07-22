#define _GNU_SOURCE

#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

#define FIFO_NM	"/home/mba/.local/share/lxc_fifo"

extern int errno;
static int err;
static const char message[] = "ABCDEFGHI";

static void err_exit(const char *msg)
{
	int err = errno;
	puts(msg);
	perror((strerror(err)));
	exit(err);
}

int main(int argc, char *argv[])
{
	int fd;
	FILE *fh;
	unsigned char key;
	const char *msg = message;

	if (!access(FIFO_NM, F_OK)) {
		unlink(FIFO_NM);
		puts("removed: "FIFO_NM);
	}

	if (mkfifo(FIFO_NM, 0666) == -1)
		err_exit("[!] mkfifo REQ FAILED");
	puts("[+] mkfifo");

	fd = open(FIFO_NM, O_WRONLY, S_IWUSR|S_IWGRP|S_IWOTH);
	if (fd==-1)
		err_exit("[!] open REQ FAILED");
	puts("[+] open WO");

	fh = fdopen(fd,"w");
	if (!fh)
		err_exit("[!] fdopen FAILED");
	puts("[+] fdopen");

	char req = *msg;
	while (req) {
		fputc(req, fh);
		req = *++msg;
	}
	fputc('', fh);

	fclose(fh);
	puts("[+] fclose");
	close(fd);
	puts("[+] close");

	return 0;
}
