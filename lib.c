#include "lib.h"
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

struct meta_block {
    size_t size;
    struct meta_block* next;
    struct meta_block* prev;
    int vacant;
};

void *BLOCK_HEAD = NULL;

// Finds a block with sufficient size;
// if such a block doesnt exist; then allocate the necessary space with sbrk
void* find_block(size_t size) {
    // Search linked list
    struct meta_block* prev = NULL;
    struct meta_block* curr = (struct meta_block*)BLOCK_HEAD;

    while (curr != NULL) {
        if (curr->vacant == 1 && size <= curr->size) {
            curr->vacant = 0;
            // TODO: truncate block to minimal length

            return sizeof(struct meta_block) + (void *)curr;
        }
        prev = curr;
        curr = curr->next;
    }

    // Create new block
    void* address = sbrk(size + sizeof(struct meta_block));
    if (address == (void *) -1) { return NULL; } // sbrk has failed

    struct meta_block *meta_pointer = (struct meta_block *)address;
    meta_pointer->size = size;
    meta_pointer->vacant = 0;
    meta_pointer->next = NULL;
    meta_pointer->prev = (void*)prev;

    void* output = (void*)meta_pointer;

    prev->next = output;

    return output + sizeof(struct meta_block);
}

void* my_malloc(size_t size) {
    if (size == 0) { return NULL; }
    if (BLOCK_HEAD == NULL) {
        // Initialize linked list
        BLOCK_HEAD = sbrk(0); 

        void* address = sbrk(size + sizeof(struct meta_block));
        if (address == (void *) -1) { return NULL; } // sbrk has failed

        struct meta_block *meta_pointer = (struct meta_block *)address;
        meta_pointer->size = size;
        meta_pointer->vacant = 0;
        meta_pointer->next = NULL;
        meta_pointer->prev = NULL;

        return BLOCK_HEAD + sizeof(struct meta_block);
    }

    void* address = find_block(size);
    return address;
}

void my_free(void* ptr) {
    struct meta_block *meta_pointer = (struct meta_block *)(ptr - sizeof(struct meta_block));
    meta_pointer->vacant = 1;

    // merge with successor if its also vacant
    if (meta_pointer->next != NULL &&  meta_pointer->next->vacant == 1) {
        meta_pointer->size = meta_pointer->size + sizeof(struct meta_block) + meta_pointer->next->size;
        meta_pointer->next = meta_pointer->next->next;
    }

    // merge with predecessor if its also vacant
    if (meta_pointer->prev != NULL && meta_pointer->prev->vacant == 1) {
        meta_pointer->prev->size = meta_pointer->prev->size + sizeof(struct meta_block) + meta_pointer->size;
        meta_pointer->prev->next = meta_pointer->next;
    }
}