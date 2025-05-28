#include "lib.h"
#include <stddef.h>
#include <unistd.h>

struct meta_block {
    size_t size;
    struct meta_block* next;
    int vacant;
};

void* my_malloc(size_t size) {
    if (size == 0) { return NULL; }

    void* address = sbrk(size + sizeof(struct meta_block));
    if (address == (void *) -1) { return NULL; } // sbrk has failed

    struct meta_block *meta_pointer = (struct meta_block *)address;
    meta_pointer->size = size;
    meta_pointer->vacant = 0;
    meta_pointer->next = NULL;

    return address + sizeof(struct meta_block);
}

void my_free(void* ptr) {

}