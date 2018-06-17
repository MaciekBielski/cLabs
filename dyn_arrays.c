#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*
 * tab - treated as an address
 * tab[i] <=> *(tab + i)
 *
 * tab[i][j] - j is the lowest-grade step
 * tab[i] - treated as an address
 * tab[i][j] <=> *(*(tab + i) + j)
 * Two steps of referencing!!
 */

int imax=3, jmax = 2;

void printArr(char *t[])
{
	for (int i = 0; i < imax; ++i)
		for (int j = 0; j < jmax; ++j)
				printf("t[%u][%u] = %c\n", i, j, t[i][j]);
}

int main(int argc, char *argv[])
{
	char **tab = malloc(imax * sizeof(char *));
	// char *tab[] -> ERROR: invalid initializer
	char letter = 'a';

	for (int i = 0; i < imax; ++i) {
		tab[i] = malloc(jmax * sizeof(char));
	}

	for (int i = 0; i < imax; ++i)
		for (int j = 0; j < jmax; ++j)
			tab[i][j] = letter++;

	printArr(tab);

	return 0;


}
