#include "balloc.h"
#include "freelist.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/mman.h>

// Define the Balloc structure
typedef struct Balloc {
    unsigned int size;
    int l;
    int u;
    void **free_lists;
} *Balloc;



Balloc bcreate(unsigned int size, int l, int u) {
    Balloc pool = (Balloc)mmap(NULL, sizeof(struct Balloc), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool == MAP_FAILED) {
        perror("Failed to allocate memory for Balloc structure");
        return NULL;
    }
    pool->size = next_power_of_two(size);
    pool->free_lists = freelistcreate(pool->size, l, u);
    pool->l = l;
    pool->u = u;
    return pool;
}

void bdelete(Balloc pool) {
    if (pool) {
        munmap(pool, sizeof(struct Balloc));
    }
}

void *balloc(Balloc pool, unsigned int size) {
    if (!pool || size > pool->size) {
        return NULL;
    }
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Failed to allocate memory");
        return NULL;
    }
    
    return mem;
}

void bfree(Balloc pool, void *mem) {
    if (pool && mem) {
        // Assuming we know the size of the allocated memory, which is not tracked in this implementation
        munmap(mem, pool->size);
    }
}

unsigned int bsize(Balloc pool, void *mem) {
    if (!pool || !mem) {
        return 0;
    }
    // This is a placeholder implementation, as the actual size tracking is not implemented
    return pool->size;
}

void bprint(Balloc pool) {
    if (pool) {
        printf("Balloc pool: size=%u, l=%d, u=%d\n", pool->size, pool->l, pool->u);
    }
}