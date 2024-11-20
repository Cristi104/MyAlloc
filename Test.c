#include "MyAlloc.h"
int main(){
    void *ptr = my_alloc(10);
    ptr = my_alloc(10);
    ptr = my_alloc(4070);
    ptr = my_alloc(10);
    *(int*)ptr = 0;
    my_free(ptr);
}