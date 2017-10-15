#ifndef ITEM_H_J4FBOXHV
#define ITEM_H_J4FBOXHV

#include <stdint.h>
#include <stddef.h>

/* borrowed from kernel code */
#define container_of(ptr, type, member) ({                      \
        const __typeof__(((type *) 0)->member) *__mptr = (ptr);     \
        (type *) ((char *) __mptr - offsetof(type, member));})

typedef struct list_head list_head;
struct list_head {
	struct list_head *next, *prev;
};

typedef struct item item;
struct item {
	int32_t val;
	list_head node;
};

#endif /* end of include guard: ITEM_H_J4FBOXHV */
