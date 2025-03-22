#include "balloc.h"
#include "freelist.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/mman.h>

// Define the Balloc structure
typedef struct Balloc {
    size_t size;
    FreeList **freeLists;
    int minOrder;
    int maxOrder;
} Balloc, *BallocPtr;

/**
 * @brief Create a new Buddy Allocator
 * 
 * @param size total size of allocator
 * @param l minimum request size
 * @param u maximum request size
 * @return BallocPtr 
 */
BallocPtr bcreate(unsigned int size, int l, int u) {
    BallocPtr pool = mmap(NULL, sizeof(struct Balloc), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    pool->size = size;
    pool->minOrder = l;
    pool->maxOrder = u;
    pool->freeLists = mmap(NULL, (u - l + 1) * sizeof(FreeList *), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool->freeLists == MAP_FAILED) {
        perror("mmap");
        munmap(pool, sizeof(struct Balloc));
        return NULL;
    }
    // Create free lists for each order
    for (int i = 0; i <= u - l; i++) {
        pool->freeLists[i] = freelistcreate(size, l, u);
        //if any fail to allocate, delete all and return NULL
        if (pool->freeLists[i] == NULL) {
            for (int j = 0; j < i; j++) {
                freelistdelete(pool->freeLists[j], l, u);
            }
            munmap(pool->freeLists, (u - l + 1) * sizeof(FreeList *));
            munmap(pool, sizeof(struct Balloc));
            return NULL;
        }
    }

    return pool;
}
/**
 * @brief delete this Buddy Allocator
 * 
 * @param pool 
 */
void bdelete(BallocPtr pool) {
    for (int i = 0; i <= pool->maxOrder - pool->minOrder; i++) {
        freelistdelete(pool->freeLists[i], pool->minOrder, pool->maxOrder);
    }
    munmap(pool->freeLists, (pool->maxOrder - pool->minOrder + 1) * sizeof(FreeList *));
    munmap(pool, sizeof(struct Balloc));
}

/**
 * @brief allocate a block of memory
 * 
 * @param pool allocator to use
 * @param size block size
 * @return void* empty block of memory
 */
void *balloc(BallocPtr pool, unsigned int size) {
    int order = pool->minOrder;
    
    // Find the smallest order that can fit the requested size
    while ((1U << order) < size) {
        order++;
    }
    if (order > pool->maxOrder) {
        return NULL;
    }
    //
    for (int i = order; i <= pool->maxOrder; i++) {
        void *block = freelistalloc(pool->freeLists[i - pool->minOrder], NULL, i, pool->minOrder);
        if (block != NULL) {
            while (i > order) {
                i--;
                void *buddy = (char *)block + (1U << i);
                freelistfree(pool->freeLists[i - pool->minOrder], NULL, buddy, i, pool->minOrder);
            }
            return block;
        }
    }
    return NULL;
}

/**
 * @brief free this block of memory
 * 
 * @param pool 
 * @param mem 
 */
void bfree(BallocPtr pool, void *mem) {
    size_t offset = (char *)mem - (char *)pool;
    int order = pool->minOrder;
    while (offset % (1U << (order + 1)) == 0 && freelistsize(pool->freeLists[order - pool->minOrder], NULL, (char *)mem + (1U << order), order, pool->maxOrder) > 0) {
        freelistfree(pool->freeLists[order - pool->minOrder], NULL, (char *)mem + (1U << order), order, pool->minOrder);
        order++;
    }
    freelistfree(pool->freeLists[order - pool->minOrder], NULL, mem, order, pool->minOrder);
}

/**
 * @brief get the size of this block of memory
 * 
 * @param pool 
 * @param mem 
 * @return unsigned int 
 */
unsigned int bsize(BallocPtr pool, void *mem) {
    size_t offset = (char *)mem - (char *)pool;
    int order = pool->minOrder;
    while (offset % (1U << (order + 1)) == 0 && freelistsize(pool->freeLists[order - pool->minOrder], NULL, (char *)mem + (1U << order), order, pool->maxOrder) > 0) {
        order++;
    }
    return 1U << order;
}

/**
 * @brief print each list's order and all blocks within
 * 
 * @param pool 
 */
void bprint(BallocPtr pool) {
    for (int i = 0; i <= pool->maxOrder - pool->minOrder; i++) {
        printf("Order %d: ", i + pool->minOrder);
        freelistprint(pool->freeLists[i], pool->minOrder, pool->maxOrder);
    }
}