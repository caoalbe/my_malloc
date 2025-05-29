#include "lib.h"
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

struct meta_block {
    size_t size;
    struct meta_block* next;
    struct meta_block* prev;
    int vacant;
};

void *BLOCK_HEAD = NULL;

// Writes a meta block at ptr location
// Returns usable memory address
void* write_meta_block(void* address, size_t size, struct meta_block* prev, struct meta_block* next, int vacant) {
    if (address == NULL) {
        // Write block at end of heap
        address = sbrk(size + sizeof(struct meta_block));
        if (address == (void *) -1) { return NULL; } // sbrk has failed
    }
    assert(address + sizeof(struct meta_block) + size <= sbrk(0)); // Ensures there is sufficient space to write

    struct meta_block *meta_pointer = (struct meta_block *)address;
    meta_pointer->size = size;
    meta_pointer->vacant = vacant;
    meta_pointer->next = next;
    meta_pointer->prev = prev;
    if (prev != NULL) { prev->next = address; }
    if (next != NULL) { next->prev = address; }

    return address + sizeof(struct meta_block);
}

void* my_malloc(size_t size) {
    if (size == 0) { return NULL; }
    if (BLOCK_HEAD == NULL) {
        // Initialize linked list
        void* data_addr = write_meta_block(NULL, size, NULL, NULL, 0);
        BLOCK_HEAD = data_addr - sizeof(struct meta_block);
        return data_addr;
    }

    // Search linked list
    struct meta_block* prev = NULL;
    struct meta_block* curr = (struct meta_block*)BLOCK_HEAD;
    while (curr != NULL) {
        if (curr->vacant == 1 && size <= curr->size) {
            curr->vacant = 0;
            if (curr->size - size > sizeof(struct meta_block)) {
                // Truncate Block to Minimal Length
                void* new_meta_block = (void*) curr + sizeof(struct meta_block) + size;
                size_t new_size = curr->size - size - sizeof(struct meta_block);
                write_meta_block(new_meta_block, new_size, curr, curr->next, 1);
                curr->size = size;
            }

            return sizeof(struct meta_block) + (void *)curr;
        }
        prev = curr;
        curr = curr->next;
    }

    // Create new block
    return write_meta_block(NULL, size, prev, NULL, 0);
}

void my_free(void* ptr) {
    // TODO: Handle null pointer
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