#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define ELEM_SZ 100*1024*1024

/*
 * tab[i][j][k] - does not work for staic arrays
 */
void printArr(char *t)
{
	size_t i, j, k;

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 3; ++j) {
			for (k = 0; k < 2; ++k, t++) {
				printf("t[%u][%u][%u] = %c\n", i, j, k, *t);
			}
		}
	}

}

int main(int argc, char *argv[])
{
	// for (int i = 0; i < 10; ++i) {
	// 	char a;
	// 	puts("Allocate 100MB? (y)");
	// 	scanf("%c", &a);
	// 	getchar();

	// 	if (a == 'y') {
	// 		ptrs[i] = malloc(ELEM_SZ);
	// 		if (!ptrs[i]) {
	// 			fprintf(stderr, "[!] Malloc error!!!\n");
	// 			exit(1);
	// 		}
	// 		memset(ptrs[i], 0xCA, ELEM_SZ);
	// 		puts("[+] OK");
	// 	} else {
	// 		break;
	// 	}
	// }
	//
	//
	char tab[4][3][2] = {
		{{'a','b'}, {'c','d'}, {'e','f'}},
		{{'g','h'}, {'i','j'}, {'k','l'}},
		{{'m','n'}, {'o','p'}, {'q','r'}},
		{{'s','t'}, {'u','v'}, {'w','x'}}
	};

	printArr(tab);

	return 0;


}
