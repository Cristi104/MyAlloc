# MyAlloc

## Description

MyAlloc is a simple memory allocation library written in the C programing language for Unix(linux) based operating systems.

The library allows proceses to dynamicaly alocate memory by using 3 basic functions (my_alloc, my_realoc and my_free) that behave similarly to libc's memory.h (malloc, realoc and free)

## Instalation

### Prerequesites

MyAlloc is a linux only library and uses make and the gcc compiler in order to build the libMyAlloc.so file

#### Ubuntu

install gcc and make
```sh
sudo apt-get install build-essentials
```

#### Other distros

install any gcc compiler version (11.4.0 is recomended) and make (4.3 si recomended)

### Building

To build just clone or download the repository and run make in the root directory

```sh
git clone https://github.com/Cristi104/MyAlloc.git
cd MyAlloc
make
make clean
```

### Testing

To make sure that the library works as intended you can run

```sh
make RunTest
make clean
```

If the test is succesful it should output the following text

```sh
gcc -shared -o libMyAlloc.so MyAlloc.o
gcc -L. -Wl,-rpath=. -Wall -o test Test.c -lMyAlloc
./test
Allocation passed
Reallocation passed
Multiple allocation passed
rm MyAlloc.o
rm test
```

## Using the library

Once you have installed and tested the library you are ready to use it, to do so simply

```c
#include "MyAlloc.h"
```

And use the 3 given functions to allocate and free memory

```c
void *my_alloc(size_t size);
void my_free(void *block);
void *my_realloc(void *block, size_t size);
```

When you are ready to compile just link to the libMyAlloc.so dynamic library

```sh
gcc -L. -Wl,-rpath=. -Wall -o executable_name source_name.c -lMyAlloc
```

## Implementation details

MyAlloc does dynamic allocation using the mmap system call to create new memory spaces available to the process. The given memory mapping is then split into memory "blocks" consisting of:
- a conitguous memory block of **n** bytes
- an 8 byte (on 64-bit machines) space that is used to store **n** and is placed just before the contiguous memory

These blocks can either be returned to the process as a pointer to the contiguous memory, or be added to an internal list of "free" blocks (when they are added to the free block list they will also be merged with adjacent blocks, if there are any free ones, and may also be returned to system if they excede a certain size).

When my_alloc is called it searches the free block list for a block that can fit the requested size, if there is one it is returned to the process, if there are no such blocks it calls mmap to create a new block that is big enough and returns it.

When my_realloc is called it searches the free block list to find a block that is adjacent to the end of the old block, if there is no such block it calls my_alloc and moves the data from the old block to the new one before it is returned, if there is a adjacent block the 2 blocks are combined and returned to the process.

When my_free is called it simply adds the given block of memory to the free blocks list, combining it with others where posible and if it is possible returning it to the operating system.