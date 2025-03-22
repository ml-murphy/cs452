#include <stdio.h>
#include <stdlib.h>
#include "freelist.h"

void test_freelist() {
    size_t size = 1024;
    int l = 4; // Minimum block size 2^4 = 16 bytes
    int u = 8; // Maximum block size 2^8 = 256 bytes

    // Create a free list
    FreeList fl = freelistcreate(size, l, u);
    if (fl == NULL) {
        fprintf(stderr, "Failed to create free list\n");
        exit(EXIT_FAILURE);
    }

    // Print the initial state of the free list
    printf("Initial state of the free list:\n");
    freelistprint(fl, l, u);

    // Allocate a block of size 2^5 (32 bytes)
    void *block1 = freelistalloc(fl, NULL, 5, l);
    if (block1 == NULL) {
        fprintf(stderr, "Failed to allocate block of size 32 bytes\n");
        freelistdelete(fl, l, u);
        exit(EXIT_FAILURE);
    }
    printf("Allocated block of size 32 bytes at %p\n", block1);

    // Allocate a block of size 2^6 (64 bytes)
    void *block2 = freelistalloc(fl, NULL, 6, l);
    if (block2 == NULL) {
        fprintf(stderr, "Failed to allocate block of size 64 bytes\n");
        freelistfree(fl, NULL, block1, 5, l);
        freelistdelete(fl, l, u);
        exit(EXIT_FAILURE);
    }
    printf("Allocated block of size 64 bytes at %p\n", block2);

    // Print the state of the free list after allocations
    printf("State of the free list after allocations:\n");
    freelistprint(fl, l, u);

    // Free the 32-byte block
    freelistfree(fl, NULL, block1, 5, l);
    printf("Freed block of size 32 bytes at %p\n", block1);

    // Free the 64-byte block
    freelistfree(fl, NULL, block2, 6, l);
    printf("Freed block of size 64 bytes at %p\n", block2);

    // Print the state of the free list after deallocations
    printf("State of the free list after deallocations:\n");
    freelistprint(fl, l, u);

    // Delete the free list
    freelistdelete(fl, l, u);
}

int main() {
    test_freelist();
    return 0;
}