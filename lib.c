#include "lib.h"
#include <stddef.h>
#include <unistd.h>

struct meta_block {
    size_t size;
    struct meta_block* next;
    int vacant;
};

void *BLOCK_HEAD = NULL;

void* my_malloc(size_t size) {
    if (size == 0) { return NULL; }
    if (BLOCK_HEAD == NULL) { BLOCK_HEAD = sbrk(0); }

    void* address = sbrk(size + sizeof(struct meta_block));
    if (address == (void *) -1) { return NULL; } // sbrk has failed

    struct meta_block *meta_pointer = (struct meta_block *)address;
    meta_pointer->size = size;
    meta_pointer->vacant = 0;
    meta_pointer->next = NULL;

    return address + sizeof(struct meta_block);
}

void my_free(void* ptr) {
    struct meta_block *meta_pointer = (struct meta_block *)ptr;
    meta_pointer->vacant = 1;
}