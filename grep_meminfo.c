#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUF_SZ 60
int main(int argc, char *argv[])
{
    FILE *fp;
    char buf[BUF_SZ];
    char *found = NULL;
    size_t nchars;
    unsigned long long mem_free = 0;

    fp = fopen("/proc/meminfo","r");
    if (!fp) {
		fprintf(stderr, "file open error\n");
        exit(EXIT_FAILURE);
    }
	printf("file opened\n");

    while (fgets(&buf, BUF_SZ, fp)) {
    	found = strstr(&buf, "MemFree: ");
    	if (found) {
			sscanf(buf, "MemFree: %llu kB", &mem_free);
			printf("avail: %llu\n", mem_free);
			break;
    	}
    }

    fclose(fp);
    exit(EXIT_SUCCESS);

	return 0;
}
