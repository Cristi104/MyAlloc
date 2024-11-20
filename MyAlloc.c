#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

static void **blocks;
static size_t max_blocks;

void insert_block(void *block){
    size_t size, i, size_iter;
    void *block_iter;
    if(block == NULL){
        return;
    }
    size = ((size_t *)block)[-1];
    for(i = 0; i < max_blocks; i++){
        block_iter = blocks[i];
        if(block_iter == NULL){
            break;
        }
        size_iter = ((size_t *)block_iter)[-1];
        if(block_iter + size_iter + sizeof(size_t) == block){
            // merge the block with a predecesor
            size_iter += sizeof(size_t) + size;
            ((size_t *)block_iter)[-1] = size_iter;
            return;
            if(block_iter + size_iter + sizeof(size_t) == blocks[i + 1]){
                // also merge with the succesor if posible
                block = block_iter;
                size = size_iter;
                block_iter = blocks[i + 1];
                if(block_iter == NULL){
                    return;
                }
                size_iter = ((size_t *)block_iter)[-1];
                size_iter += sizeof(size_t) + size;
                ((size_t *)block_iter)[-1] = size_iter;
                memcpy(&blocks[i + 1], &blocks[i + 2], (max_blocks - i - 2) * sizeof(void*));
            }
        }
        if(block + size + sizeof(size_t) == block_iter){
            // merge with the block's succesor
            size += size_iter + sizeof(size_t);
            ((size_t *)block)[-1] = size;
            blocks[i] = block;
            return;
        }
        // if it can't be merged just add it to the array so that it's still in ascending order
        if(blocks[max_blocks - 1] != NULL){
            // resize block array to fit
        }
        if(block_iter < block && block < blocks[i + 1]){
            memcpy(blocks[i + 2], blocks[i + 1], (max_blocks - i - 2) * sizeof(void *));
            blocks[i + 1] = block;
            return;
        }
    }
    blocks[i] = block;
}

void print_blocks(){
    void *block;
    for(size_t i = 0; i < max_blocks; i++){
        block = blocks[i];
        if(block == NULL){
            return;
        }
        printf("Block: %p, of size: %ld, at index: %ld, overhead bytes: %ld\n",block, ((size_t *)block)[-1], i, sizeof(size_t));
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
            block_size -= size + sizeof(size_t);
            block += size + sizeof(size_t);
            ((size_t *)block)[-1] = block_size;
            blocks[i] = block;
        }else{
            // if the block is to small to be split remove it from the free blocks array
            memcpy(&blocks[i], blocks[i + 1], (max_blocks - i - 1) * sizeof(void *));
        }
        printf("Allocated block at: %p of size: %ld, overhead bytes: %ld\n", ret, ((size_t *)ret)[-1], sizeof(size_t));
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
    printf("Allocated block at: %p of size: %ld, overhead bytes: %ld\n", ret, ((size_t *)ret)[-1], sizeof(size_t));
    // print_blocks();
    return ret;
}

void my_free(void *block){
    insert_block(block);
    printf("Freed block at: %p of size: %ld, overhead bytes: %ld\n", block, ((size_t *)block)[-1], sizeof(size_t));
}

void *my_realloc(void *block, size_t size){

    return NULL;
}