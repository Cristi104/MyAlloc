#include "MyAlloc.h"
#include <assert.h>
#include <stdio.h>
// #include <stdtype.>

int main(){
    char* ptr1, *ptr2;

    // alloc test
    ptr1 = my_alloc(100);
    ptr1[0] = 11;
    ptr1[99] = 12;
    assert(((size_t *)ptr1)[-1] == 100);
    my_free(ptr1);
    printf("Allocation passed\n");

    // realoc test
    ptr1 = my_alloc(100);
    ptr1[0] = 21;
    ptr1[99] = 22;

    ptr1 = my_realloc(ptr1, 1000);
    assert(((size_t *)ptr1)[-1] == 1000);
    assert(ptr1[0] == 21);
    assert(ptr1[99] == 22);
    my_free(ptr1);
    printf("Reallocation passed\n");

    // multiple alloc test
    ptr1 = my_alloc(2040);
    ptr1[0] = 31;
    ptr1[2039] = 32;
    
    ptr2 = my_alloc(2040);
    ptr2[0] = 33;
    ptr2[2039] = 34;

    my_free(ptr1);
    my_free(ptr2);
    printf("Multiple allocation passed\n");

}