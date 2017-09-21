#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

extern int errno;
static int err;

/*
 * Find a pivot element and place all smaller elements on the left and larger
 * on the right. After, do the same for both halves. The pivot can be choosen
 * by different methods, here it is an element which is closest to avg.
 */
static int32_t *pivot(int32_t *arr, size_t len)
{
	int64_t sum = 0, avg;
	int32_t *out = arr;
	uint32_t tmp, min_diff = UINT32_MAX;

	if (len==1) {
		out = arr;
		goto pivot_out;
	}

	for (int32_t *p = arr; p < arr+len; ++p)
		sum += *p;
	avg = sum/len;

	for (int32_t *p = arr; p < arr+len; ++p) {
		tmp = abs(*p-avg);
		if (tmp < min_diff) {
			min_diff = tmp;
			out = p;
		}
	}

pivot_out:
	printf("\t\tpivot = %u\n", *out);
	return out;
}

// TODO: const pointers
static inline void swap(int32_t *p, int32_t *q)
{
	if (p==q)
		return;
	int32_t tmp = *p;
	*p = *q;
	*q = tmp;
}
/*
 * Copy pivot at the beginning temporarily. Searching in two parts in turns.
 * Starting from beginning look for fromLeft>**piv, when found start from the
 * end to look for fromRight<=**piv and go backwards. When found a pair,
 * replace the values. Stop once fromRight==fromLeft and put the pivot between
 * them.
 */
static void arrange(int32_t **piv, int32_t *arr, size_t len)
{
	int *lh = arr, *rh = arr+len;	//init out-of-scope

	if (len<=1)
		goto arrange_out;

	/* temporarily put the pivot at the beginning */
	swap(arr, *piv);
	*piv = arr;

	while (lh<rh) {
		/* increment left side until higher found */
		while (lh<rh) {
			++lh;
			if (*lh>**piv)	//hit
				break;
		}
		printf("\t lh stopped at: %u\n", *lh);
		/* decrement right side until lower or equal found */
		while (lh<rh) {
			--rh;
			if (*rh<=**piv)	//hit
				break;
		}
		printf("\t rh stopped at: %u\n", *rh);

		if (lh<rh) {
			swap(lh, rh);
			printf("\t swapped: %u and %u\n", *lh, *rh);
		}
	}
	/*
	 * corner case: lh may be at rh=arr+len
	 * or
	 * lh==rh but they may point to element either larger or smaller than pivot
	 * value so if larger then swap with previous element
	 */
	if (lh==arr+len || *lh>*arr)
		--lh;
	/* put the pivot back */
	swap(*piv, lh);
	*piv = lh;

arrange_out:
	return;
}

/* find first 0 in registry (not pivot yet) */
static inline size_t find_start(size_t *reg, size_t min, size_t max) {
	size_t off;

	if (min>=max)
		return max;

	for(off=min; off<max; ++off)
		if(0 == *(reg+off))
			break;

	return off;
}

/* find first non-0 in registry (stop pivot) */
static size_t find_end(size_t *reg, size_t min, size_t max) {
	size_t off;

	if (min>=max)
		return max;

	for(off=min; off<max; ++off)
		if(0 != *(reg+off))
			break;

	return off;
}

static void print_array(uint32_t *arr, size_t len, size_t piv) {
	for (int i = 0; i < len; ++i)
		if (i==piv)
			printf("(%d) ", arr[i]);
		else
			printf(" %d  ", arr[i]);
	puts("");
}

static void print_reg(size_t *arr, size_t len) {
	for (int i = 0; i < len; ++i)
		printf(" %lu  ", arr[i]);
	puts("");
}

/* iterative version instead of recursive one */
static void quick_sort(int32_t *arr, size_t len)
{
	/* convention [start, end) */
	size_t start = 0, end;
	int32_t *p = pivot(arr, len);				//pivot
	size_t *ppos = calloc(len, sizeof(size_t)); //pivots register

	if (!ppos) {
		err = errno;
		perror(strerror(err));
		exit(err);
	}

	for (size_t i = 1; start<len; ++i) {
		while (1) {
			print_reg(ppos, 14);
			/* find exclusive end */
			end = find_end(ppos, start, len);
			printf("start: %lu, end: %lu\n", start, end);
			if (end-start<2) {
				/* mark to not be visited again */
				ppos[start] = i;
				goto next_sub;
			}

			p = pivot(arr+start, end-start);
			arrange(&p, arr+start, end-start);
			print_array(arr, 14, p-arr);
			/* mark the pivot in registry */
			ppos[p-arr]	= i+1;

next_sub:
			start = find_start(ppos, end+1, len);
			if (start>(len-2))
				break;
		}
		start = find_start(ppos, 0, len);
	}

	free(ppos);
}

int32_t main()
{
	int32_t input[] = {10, 90, 50, 40, 70, 10, 30, 80, 80, 30, 90, 40, 50, 70};
	int32_t input2[] = {10, 10, 10, 10, 10};
	int32_t input3[] = {9, -8, 7, -6, 5, -4, 3, -2, 1};

	print_array(input, 14, 14);
	quick_sort(input, 14);
	print_array(input, 14, 14);

	print_array(input2, 5, 5);
	quick_sort(input2, 5);
	print_array(input2, 5, 5);

	print_array(input3, 9, 9);
	quick_sort(input3, 9);
	print_array(input3, 9, 9);
}
