#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

static void **blocks;
static size_t max_blocks;

void print_blocks(){
    void *block;
    for(size_t i = 0; i < max_blocks; i++){
        block = blocks[i];
        if(block == NULL){
            return;
        }
        printf("Block: %p, of size: %ld, at index: %ld\n",block, ((size_t *)block)[-1], i);
    }
}

void *my_alloc(size_t size){
    size_t block_size, i, alloc_size;
    void *block, *ret, *alloc;
    if(blocks == NULL){
        blocks = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(blocks == MAP_FAILED){
            perror(NULL);
            exit(errno);
        }
        max_blocks = PAGE_SIZE / sizeof(void *);
        memset(blocks, 0, PAGE_SIZE);
        printf("Created new mapping for block storage at: %p of count: %ld\n", blocks, max_blocks);
    }
    for(i = 0; i < max_blocks; i++){
        block = blocks[i];
        if(block == NULL){
            break;
        }
        block_size = ((size_t *)block)[-1];
        if(block_size < size){
            continue;
        }
        ret = block;
        if(block_size - size > sizeof(size_t)){
            // split block into 2 blocks, one of size bytes to be returned and one with the remaining bytes (- the one size_t overhead for length storage)
            ((size_t *)block)[-1] = size;
            block_size -= size - sizeof(size_t);
            block += size + sizeof(size_t);
            ((size_t *)block)[-1] = block_size;
            blocks[i] = block;
        }else{
            // if the block is to small to be split remove it from the free blocks array
            memcpy(&blocks[i], blocks[i + 1], (max_blocks - i - 1) * sizeof(void *));
        }
        printf("Returned block at: %p of size: %ld\n", ret, ((size_t *)ret)[-1]);
        return ret;
    }
    // if there is no suitable free blocks alocate new ones;
    alloc_size = PAGE_SIZE * (1 + (size + sizeof(size_t)) / PAGE_SIZE);
    alloc = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(alloc == MAP_FAILED){
        perror(NULL);
        exit(errno);
    }
    printf("Created new mapping at: %p of size: %ld\n", alloc, alloc_size);
    ret = alloc + sizeof(size_t);

    if(alloc_size - size > sizeof(size_t)){
        // split the block into 2 blocks one to be returned and one to be added to the free blocks list
        ((size_t *)alloc)[0] = size;
        block_size = alloc_size - size - sizeof(size_t) * 2;
        block = alloc + size + sizeof(size_t) * 2;
        ((size_t *)block)[-1] = block_size;
        blocks[i] = block;
    } else {
        // if the block is to small to be split just set the size and return it
        ((size_t *)alloc)[0] = alloc_size - sizeof(size_t);
    }
    printf("Returned block at: %p of size: %ld\n", ret, ((size_t *)ret)[-1]);
    print_blocks();
    return ret;
}

void my_free(void *block){

}

void *my_realloc(void *block, size_t size){

    return NULL;
}