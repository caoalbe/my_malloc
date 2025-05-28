#include "lib.h"
#include <stddef.h>
#include <unistd.h>

void* my_malloc(size_t size) {
    if (size == 0) { return NULL; }

    void* output = sbrk(size);
    if (output == (void *) -1) { return NULL; } // sbrk has failed

    return output;
}

void my_free(void* ptr) {

}