/*
 * Elements of a generated linked list are sorted by mergelist.
 * Max length: UINT32_MAX
 *
 * N = #elements
 * N needs to be broken down to binary form in order to find two lists at each
 * level: main list (first max multiple-of-divider) and leftovers (all others)
 *
 * merge - assumes that a list can be shorter or equal to 3rd argument
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "item.h"
extern int errno;
static int err;

/*
 * We pretend here that the list is scattered through memory
 *
 * Convention: first element does not have link back to head element
 */
static list_head * items_list(const int32_t *in, const size_t len)
{
	item *out = calloc(len+1, sizeof(item));
	item *prv = NULL;
	item *it = NULL;
	const int32_t *vals = NULL;

	if (!out) {
		err = errno;
		perror(strerror(err));
		exit(err);
	}

	/* head init */
	it = out+1;
	out->node.next = &it->node;
	/* linking rest of the elements */
	for (vals = in; it < (out+1+len); ++it, ++vals) {

		it->val = *vals;
		if (prv) {
			it->node.prev = &prv->node;
			prv->node.next = &it->node;
		}

		prv = it;
	}

	return &out->node;
}

static void print_list(const list_head *head)
{
	const list_head *tmp = head;
	item *it;

	while (tmp->next) {
		it = container_of(tmp->next, item, node);
		printf("%d ", it->val);
		tmp = &it->node;
	}
	puts("");
}

/* transforms the length to binary value, returns position of MSB */
static uint8_t length_breakdown(uint32_t len, bool res[])
{
	uint8_t shift = 0;

	printf("len: %u, ", len);
	while (len) {
		res[shift++] = len%2;
		len >>= 1;
	}
	--shift;

	for (int16_t i = shift; i >=0; --i) {
		printf("%u", res[i]);
		if (!(i%4))
			printf(" ");
	}
	puts("");

	return shift;
}

/*
 * Advance till the end -> terminate and reset the pointer the beginning of a
 * next sublist
 *
 * tmp touches the real data
 *
 * The *l points to the end of sublist and, (*l)->next has to be stored in a
 * separate object that will stay valid after the function returns(!) and then
 * will be NULLed to terminate the sublist.
 *
 * Also the 'l' points to external pointer that has to stay valid, thus it is
 * re-validated so that it can continue pointing to remainding elements.
 */
static void link_advance_and_terminate(list_head **data, const size_t steps,
		list_head *subhead)
{
	list_head *data_p = *data;

	// advance by all steps except the last one
	for (size_t i = 0; i < steps; ++i) {
		if (data_p->next) {
			*data = data_p->next;
			data_p = *data;
		} else {
			perror("NULL link");
			exit(EXIT_FAILURE);
		}
	}
	// (*data_p) is to be terminated
	subhead->next = data_p->next;
	data_p->next = NULL;
	(*data) = subhead;

	// first elem of next sublist does not have a link to the 'head'
	if (subhead->next)
		subhead->next->prev = NULL;
}

/*
 * Merges two sublists, sets up lh to the beginning of merged one
 * and rh to the end
 *
 * lh and rh are buffers, don't change them while iterating over them, copy the
 * old value first instead them move it, then use the old value for relinking
 */
static void merge(list_head *lh, list_head *rh, list_head *next_l, void *last_r)
{
	list_head out = {0},
			  *init_lh = lh,
			  *init_rh = rh,
			  *rest = NULL,
			  *prev_lh_next = NULL,
			  *prev_rh_next = NULL,
			  *prev_rest_next = NULL,
			  *saved_rh_next = NULL;
	list_head **tmp = &out.next;	// pointer that sets the pointer
	item *l, *r;

	if (!lh || !rh)
		return;

	prev_lh_next = lh->next;
	prev_rh_next = rh->next;

	while (prev_lh_next && prev_rh_next) {
		l = container_of(prev_lh_next, item, node);
		r = container_of(prev_rh_next, item, node);
		if (l->val <= r->val) {
			fprintf(stderr, "l: %d <= r: %d\n", l->val, r->val);
			*tmp = prev_lh_next;	// setup a pointer to next node
			lh = prev_lh_next;		// move the queue pointer
			tmp = &lh->next;		// get access to the next pointer to set up
			prev_lh_next = *tmp;	// save the old value before it gets overwritten
		} else {
			fprintf(stderr, "l: %d > r: %d\n", l->val, r->val);
			*tmp = prev_rh_next;
			rh = prev_rh_next;
			tmp = &rh->next;
			prev_rh_next = *tmp;
		}

		// if (tmp.next)
		// 	tmp.next->prev = tmp;
	}

	/*
	 * the sublist that is finished has Xh->next == NULL so choose the other
	 * one
	 */
	rest = lh->next ? lh : rh;
	prev_rest_next = lh->next ? prev_lh_next : prev_rh_next;

	while (rest->next) {
		*tmp = prev_rest_next;

		// remember pointer to last element
		// cannot write directly to init_rh since rest may point to the same location
		saved_rh_next = prev_rest_next;		// prev_prev_rest_next
		rest = prev_rest_next;
		tmp = &rest->next;
		prev_rest_next = *tmp;
	}


	// connect last element with the next sublist
	if (next_l->next)
		rest->next = next_l->next;
	// link the beginning of a sorted sublist with the end of a previous sorted
	// sublist
	if (last_r)
		((list_head *)last_r)->next = out.next;

	// update the rh.next buffer to point to the last element of this sorted sublist
	init_rh->next = saved_rh_next;
	// update the lh.next buffer to point to the first element of this sorted sublist
	init_lh->next = out.next;

	// Compensation: unlink first's element prev
	if (out.next)
		out.next->prev = NULL;
}
/*
 * Non-recursive merge sort, for simplicity we pass len. The binary
 * representation tells at each pairing level if there are any leftover
 * elements at the end after pairing. Pairing starts from the beginning.
 */
static void merge_sort(list_head *head, const size_t len)
{
	bool binval[32] = {0}, has_leftovers = false;
	uint8_t msb_pos;
	list_head out = *head;

	msb_pos = length_breakdown(len, binval);
	printf("lsb pos: %d \n", msb_pos);

	/* outer loop changes order of sublists to be paired */
	for (uint8_t p = 0; p < msb_pos; ++p) {
		const size_t in_order = 1<<p;
		const size_t out_order = 1<<(p+1);
		const size_t n_pairs = len>>(p+1);
		list_head *tmp = &out, l_link = {0}, r_link = out, next_l = out;
		void *last_r = NULL;
		void *leftovers_last_r = NULL;

		/* inner loop performs pairing */
		for (size_t i_pair = 0; i_pair < n_pairs; ++i_pair) {
			l_link = next_l;					// l_link.next -> first sublist
			/*
			 * last sorted sublist will be merged with leftovers so it can be
			 * completely re-sorted again, therefore it will need a link to
			 * the tail of last-1 pair
			 */
			if (n_pairs-1 == i_pair)
				leftovers_last_r = r_link.next;

			link_advance_and_terminate(&tmp, in_order, &r_link);
			link_advance_and_terminate(&tmp, in_order, &next_l);
			// TODO: fails at second round
			merge(&l_link, &r_link, &next_l, last_r);

			last_r = r_link.next;
			if (!i_pair)
				out.next = l_link.next;	// out.next -> start of the main list
			print_list(&out);
		}
		/*
		 * deal with leftovers, merge them with last merged sublist, once they
		 * are detected each next iteration needs to merge them
		 */
		if (has_leftovers || binval[p]) {
			has_leftovers = true;
			/*
			 * there will be a leftovers list of max order in_order-1,
			 * NULL-terminated
			 */
			// previous ordered sublist needs to be terminated
			((list_head *)last_r)->next = NULL;
			// set up a sublist of leftovers
			r_link.next = tmp->next;
			l_link.next = ((list_head *)leftovers_last_r)->next;
			// merged sublist needs also to be properly terminated
			next_l.next = NULL;
			merge(&l_link, &r_link, &next_l, leftovers_last_r);
			print_list(&out);
		}
	}

	head->next = out.next;
}

int main(int argc, char *argv[])
{

	int32_t input1[] = {10, 90, 50, 40, 70, 10, 30, 80, 80, 30, 90, 40, 50, 70};
	// int32_t input1[] = {90, 10, 50, 40, 20};
	int32_t input2[] = {10, 10, 10, 10, 10};
	int32_t input3[] = {9, -8, 7, -6, 5, -4, 3, -2, 1};

	list_head *items1 = items_list(input1, 14);
	print_list(items1);
	merge_sort(items1, 14);
	print_list(items1);

	list_head *items3 = items_list(input3, 9);
	print_list(items3);
	merge_sort(items3, 9);
	print_list(items3);
	return 0;
}
