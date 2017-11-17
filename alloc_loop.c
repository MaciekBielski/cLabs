#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define ELEM_SZ 100*1024*1024

char *ptrs[10] = {0};

int main(int argc, char *argv[])
{
	for (int i = 0; i < 10; ++i) {
		char a;
		puts("Allocate 100MB? (y)");
		scanf("%c", &a);
		getchar();

		if (a == 'y') {
			ptrs[i] = malloc(ELEM_SZ);
			if (!ptrs[i]) {
				fprintf(stderr, "[!] Malloc error!!!\n");
				exit(1);
			}
			memset(ptrs[i], 0xCA, ELEM_SZ);
			puts("[+] OK");
		} else {
			break;
		}
	}
	return 0;
}
