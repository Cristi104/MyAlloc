#include "MyAlloc.h"
int main(){
    void *ptr = my_alloc(10);
    my_free(ptr);
}