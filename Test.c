#include "MyAlloc.h"
int main(){
    void *ptr = my_alloc(10);
    ptr = my_realloc(ptr, 500);
    ptr = my_realloc(ptr, 5000);
    my_free(ptr);
}