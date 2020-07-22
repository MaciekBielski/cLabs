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
	char buff[2];

	fd = open(FIFO_NM, O_RDONLY, S_IRUSR|S_IRGRP|S_IROTH);
	if (fd==-1)
		err_exit("[!] open FAILED");
	puts("[+] open RO");

	fh = fdopen(fd,"r");
	if (!fh)
		err_exit("[!] fdopen FAILED");
	puts("[+] fdopen");

	memset(buff, '\0', 2);
	for(size_t j=0;j<20;) {
		buff[0] = (char)fgetc(fh);
		if (buff[0]==EOF || buff[0] == '') {
			puts("QUIT");
			break;
		}
		puts(buff);
	}
	fclose(fh);
	puts("[+] fclose");
	close(fd);
	puts("[+] close");

	return 0;
}
