#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "deq.h"

Deq init_deq();
void test_deq_head();
void test_deq_tail();
void test_deq_map();
void test_deq_del();
void print_data(Data d);

int main() {
    test_deq_head();
    test_deq_tail();
    test_deq_map();
    // test_deq_del(); // all other methods test deletion
    printf("All tests passed!\n");

    return 0;
}

Deq init_deq_head() {
    Deq q = deq_new();
    deq_head_put(q, "seven");
    deq_head_put(q, "six");
    deq_head_put(q, "five");
    deq_head_put(q, "four");
    deq_head_put(q, "three");
    deq_head_put(q, "two");
    deq_head_put(q, "one");
    return q;
}

Deq init_deq_tail() {
    Deq q = deq_new();
    deq_tail_put(q, "one");
    deq_tail_put(q, "two");
    deq_tail_put(q, "three");
    deq_tail_put(q, "four");
    deq_tail_put(q, "five");
    deq_tail_put(q, "six");
    deq_tail_put(q, "seven");
    return q;
}

void test_deq_head() {
    Deq q = init_deq_head();
    char *full_deq = deq_str(q, 0);
    assert(strcmp(full_deq, "one two three four five six seven") == 0);

    char *result = (char *)deq_head_get(q);
    assert(strcmp(result, "one") == 0);
    result = deq_head_ith(q, 2);
    assert(strcmp(result, "four") == 0);
    result = deq_head_rem(q, "four");
    assert(strcmp(result, "four") == 0);
}

void test_deq_tail() {
    Deq q = init_deq_tail();
    char *full_deq = deq_str(q, 0);
    assert(strcmp(full_deq, "one two three four five six seven") == 0);

    char *result = deq_tail_get(q);
    assert(strcmp(result, "seven") == 0);
    result = deq_tail_ith(q, 2);
    assert(strcmp(result, "four") == 0);
    result = deq_tail_rem(q, "four");
    assert(strcmp(result, "four") == 0);
}

void test_deq_map() {
    Deq q = init_deq_head();
    deq_map(q, print_data);

    deq_del(q, 0);
}

void print_data(Data d) {
    printf("%s\n", (char *)d);
}

