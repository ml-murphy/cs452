#include <stdio.h>
#include <stdlib.h>
#include "balloc.h"

void test_balloc_create_delete() {
    printf("Testing bcreate and bdelete...\n");
    Balloc pool = bcreate(1024, 4, 8);
    if (pool == NULL) {
        fprintf(stderr, "Failed to create memory pool\n");
        exit(EXIT_FAILURE);
    }
    bprint(pool);
    bdelete(pool);
    printf("bcreate and bdelete tests passed.\n");
}

void test_balloc_alloc_free() {
    printf("Testing balloc and bfree...\n");
    Balloc pool = bcreate(1024, 4, 8);
    if (pool == NULL) {
        fprintf(stderr, "Failed to create memory pool\n");
        exit(EXIT_FAILURE);
    }

    void *block1 = balloc(pool, 32);
    if (block1 == NULL) {
        fprintf(stderr, "Failed to allocate block of size 32 bytes\n");
        bdelete(pool);
        exit(EXIT_FAILURE);
    }
    printf("Allocated block of size 32 bytes at %p\n", block1);

    void *block2 = balloc(pool, 64);
    if (block2 == NULL) {
        fprintf(stderr, "Failed to allocate block of size 64 bytes\n");
        bfree(pool, block1);
        bdelete(pool);
        exit(EXIT_FAILURE);
    }
    printf("Allocated block of size 64 bytes at %p\n", block2);

    bprint(pool);

    bfree(pool, block1);
    printf("Freed block of size 32 bytes at %p\n", block1);

    bfree(pool, block2);
    printf("Freed block of size 64 bytes at %p\n", block2);

    bprint(pool);

    bdelete(pool);
    printf("balloc and bfree tests passed.\n");
}

void test_balloc_size() {
    printf("Testing bsize...\n");
    Balloc pool = bcreate(1024, 4, 8);
    if (pool == NULL) {
        fprintf(stderr, "Failed to create memory pool\n");
        exit(EXIT_FAILURE);
    }

    void *block1 = balloc(pool, 32);
    if (block1 == NULL) {
        fprintf(stderr, "Failed to allocate block of size 32 bytes\n");
        bdelete(pool);
        exit(EXIT_FAILURE);
    }
    printf("Allocated block of size 32 bytes at %p\n", block1);

    unsigned int size1 = bsize(pool, block1);
    printf("Size of allocated block: %u bytes\n", size1);
    if (size1 != 32) {
        fprintf(stderr, "Incorrect block size: expected 32, got %u\n", size1);
        bfree(pool, block1);
        bdelete(pool);
        exit(EXIT_FAILURE);
    }

    bfree(pool, block1);
    printf("Freed block of size 32 bytes at %p\n", block1);

    bdelete(pool);
    printf("bsize tests passed.\n");
}

int main() {
    test_balloc_create_delete();
    test_balloc_alloc_free();
    test_balloc_size();
    printf("All tests passed.\n");
    return 0;
}
