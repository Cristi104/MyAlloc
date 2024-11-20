#ifndef MY_ALLOC_H
#define MY_ALLOC_H

#include <stddef.h>
extern void *my_alloc(size_t size);

extern void my_free(void *block);

extern void *my_realloc(void *block, size_t size);

#endif