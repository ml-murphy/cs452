#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>

void *mmalloc(size_t size) {
    void *p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
        fprintf(stderr, "Memory allocation with mmap failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

void mmfree(void *p, size_t size) {
    if (munmap(p, size) == -1) {
        fprintf(stderr, "Memory deallocation with munmap failed\n");
    }
}

size_t divup(size_t n, size_t d) {
    return (n + d - 1) / d;
}

size_t bits2bytes(size_t bits) {
    return divup(bits, bitsperbyte);
}

size_t e2size(int e) {
    return (size_t)1 << e;
}

int size2e(size_t size) {
    int e = 0;
    while ((size_t)(1 << e) < size) {
        e++;
    }
    return e;
}

void bitset(void *p, int bit) {
    ((unsigned char *)p)[bit / bitsperbyte] |= (1 << (bit % bitsperbyte));
}

void bitclr(void *p, int bit) {
    ((unsigned char *)p)[bit / bitsperbyte] &= ~(1 << (bit % bitsperbyte));
}

void bitinv(void *p, int bit) {
    ((unsigned char *)p)[bit / bitsperbyte] ^= (1 << (bit % bitsperbyte));
}

int bittst(void *p, int bit) {
    return ((unsigned char *)p)[bit / bitsperbyte] & (1 << (bit % bitsperbyte));
}