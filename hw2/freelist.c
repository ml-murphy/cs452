#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>
#include "freelist.h"
#include "bbm.h"
#include "utils.h"


typedef struct FreeListNode {
    struct FreeListNode *next;
} FreeListNode;

typedef struct FreeListStruct {
    FreeListNode *head;
    BBM bitmap;
} FreeListStruct;

typedef FreeListStruct *FreeList;
/**
 * @brief Create a new FreeList, which is a linked list 
 * of free blocks, and an associated bitmap to keep track.
 * 
 * @param size total size of the memory pool
 * @param l min block request size
 * @param u max block request size
 * @return FreeList 
 */
FreeList freelistcreate(size_t size, int l, int u) {
    FreeList fl = mmap(NULL, sizeof(FreeListStruct), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (fl == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    fl->head = NULL;
    fl->bitmap = bbmcreate(size, u);
    if (fl->bitmap == NULL) {
        munmap(fl, sizeof(FreeListStruct));
        return NULL;
    }

    // Initialize the free list with nodes
    size_t block_size = e2size(l);
    size_t num_blocks = size / block_size;
    void *base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base == MAP_FAILED) {
        perror("mmap for free list nodes failed");
        bbmdelete(fl->bitmap);
        munmap(fl, sizeof(FreeListStruct));
        return NULL;
    }

    for (size_t i = 0; i < num_blocks; i++) {
        FreeListNode *node = (FreeListNode *)((char *)base + i * block_size);
        node->next = fl->head;
        fl->head = node;
    }

    printf("FreeList created with %zu blocks of size %zu\n", num_blocks, block_size);

    return fl;
}


/**
 * @brief Delete this freelist and bitmap
 * 
 * @param f 
 * @param l 
 * @param u 
 */
void freelistdelete(FreeList f, int l, int u) {
    bbmdelete(f->bitmap);
    munmap(f, sizeof(FreeListStruct));
}

/**
 * @brief allocate a proper length list given the exponent
 * 
 * @param f 
 * @param base 
 * @param e 
 * @param l 
 * @return void* 
 */
void *freelistalloc(FreeList f, void *base, int e, int l) {
    if (f->head == NULL) {
        return NULL;
    }
    FreeListNode *node = f->head;
    f->head = node->next;
    printf("Allocating block at %p\n", node); 
    bbmset(f->bitmap, base, node, e);
    return node;
}

/**
 * @brief free this allocated block of memory
 * 
 * @param f 
 * @param base 
 * @param mem 
 * @param e 
 * @param l 
 */
void freelistfree(FreeList f, void *base, void *mem, int e, int l) {
    printf("Freeing block at %p\n", mem); 
    bbmclr(f->bitmap, base, mem, e);
    FreeListNode *node = (FreeListNode *)mem;
    node->next = f->head;
    f->head = node;
}

/**
 * @brief free this entire FreeList
 * 
 * @param f 
 * @param base 
 * @param mem 
 * @param l 
 * @param u 
 * @return int 
 */
int freelistsize(FreeList f, void *base, void *mem, int l, int u) {
    int size = 0;
    FreeListNode *node = f->head;
    while (node != NULL) {
        size++;
        node = node->next;
    }
    return size;
}

/**
 * @brief print out the FreeList and bitmap
 * 
 * @param f 
 * @param l 
 * @param u 
 * @return * void 
 */
void freelistprint(FreeList f, int l, int u) {
    FreeListNode *node = f->head;
    printf("FreeList: ");
    while (node != NULL) {
        printf("%p -> ", node);
        node = node->next;
    }
    printf("NULL\n");
    bbmprt(f->bitmap);
}