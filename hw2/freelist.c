#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "freelist.h"

typedef struct FreeListNode {
    struct FreeListNode *next;
} FreeListNode;

struct FreeList {
    FreeListNode *head;
    size_t size;
};
\
FreeList **freelistcreate(size_t size, int l, int u) {
    FreeList f = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (f == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
    ((struct FreeList *)f)->head = NULL;
    ((struct FreeList *)f)->size = size;
    return f;
}

void freelistdelete(FreeList f, int l, int u) {
    if (munmap(f, ((struct FreeList *)f)->size) == -1) {
        perror("munmap");
    }
}

void *freelistalloc(FreeList f, void *base, int e, int l) {
    struct FreeList *fl = (struct FreeList *)f;
    if (fl->head == NULL) {
        return NULL;
    }
    FreeListNode *node = fl->head;
    fl->head = node->next;
    return node;
}

void freelistfree(FreeList f, void *base, void *mem, int e, int l) {
    struct FreeList *fl = (struct FreeList *)f;
    FreeListNode *node = (FreeListNode *)mem;
    node->next = fl->head;
    fl->head = node;
}

int freelistsize(FreeList f, void *base, void *mem, int l, int u) {
    struct FreeList *fl = (struct FreeList *)f;
    int count = 0;
    FreeListNode *node = fl->head;
    while (node != NULL) {
        count++;
        node = node->next;
    }/*-    */
    return count;
}

void freelistprint(FreeList f, int l, int u) {
    struct FreeList *fl = (struct FreeList *)f;
    FreeListNode *node = fl->head;
    while (node != NULL) {
        printf("%p -> ", node);
        node = node->next;
    }
    printf("NULL\n");
}
