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

    void* output = sbrk(size + sizeof(struct meta_block));
    if (output == (void *) -1) { return NULL; } // sbrk has failed

    struct meta_block *meta_pointer = (struct meta_block *)output;
    meta_pointer->size = size;
    meta_pointer->vacant = 99;
    meta_pointer->next = NULL;

    return output + sizeof(struct meta_block);
}

void my_free(void* ptr) {

}