#ifndef MAIN_H
#define MAIN_H

typedef void *Deq;
typedef void *Data;

extern Deq init_deq();
extern void test_deq_head();
extern void test_deq_tail();
extern void test_deq_map();
extern void test_deq_del();
void print_data(Data d);

#endif