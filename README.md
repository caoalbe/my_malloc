# my_malloc
An implementation of the `malloc` and `free` functions from the C standard library.  This is an exercise in how memory management and allocation behaves at the low level using the `sbrk` system calls

**Contribution and feedback are welcomed!**

# Features
**Heap Management**<br>
Using `sbrk`, we allocate memory from the process break directly

**Metadata**<br>
Each block of allocated memory contains metadata which keeps information about the block size and its vacancy.  The metadata blocks are stored in a linked list fashion

**Block Splitting**<br>
If a requested memory allocation is less than a free block's memory; then that block is split and the remainder is used for another free block

**Block Coalescing**<br>
If consecutive blocks in memory are not in use; then they are merged together to prevent fragmentation


# Usage
You can use this allocator in your own programs:
```
#include "lib.h"
int main() {
    // Allocates memory for 10 integers
    int *ptr = my_malloc(10 * sizeof(int));

    ptr[0] = 5;
    ptr[1] = 29;
    ptr[2] = 2025;

    // Frees the memory
    my_free(ptr);
}
```