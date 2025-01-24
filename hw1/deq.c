#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deq.h"
#include "error.h"

// indices and size of array of node pointers
typedef enum {Head,Tail,Ends} End;

typedef struct Node {
  struct Node *np[Ends];        // next/prev neighbors
  Data data;
} *Node;

typedef struct {
  Node ht[Ends];                // head/tail nodes
  int len;
} *Rep;

static Rep rep(Deq q) {         // cast and check
  if (!q) ERROR("zero pointer");
  return (Rep)q;
}

static void put(Rep r, End e, Data d) {
  // initialize new node
  Node n=(Node)malloc(sizeof(*n));
  if (!n) ERROR("malloc() failed");

  //set data and neighbors of new node
  n->data=d;
  n->np[e]=0;
  if (!r->ht[Head]) { // if empty, head=tail=new_node
    r->ht[Head]=n;
    r->ht[Tail]=n;
  } else {
    /* "Operations on opposite ends are symmetrical." 
        We use the `End` enum's integer representation 
        to determine how `n`'s next/prev pointers 
        (and obviously, `r`'s head/tail pointers) 
        should be updated. 
    */
    n->np[!e]=r->ht[e]; // "new_node.np[(e == Head ? Tail : Head)] = r->ht[e]"
    r->ht[e]->np[e]=n; 
    r->ht[e]=n;
  }
  
  //increment length of deque
  r->len++;
}
static Data ith(Rep r, End e, int i)  {
  Node n = r->ht[e];
  for (int j = 0; j < i; j++) {
    n = n->np[!e];
  }
  return n->data;
}
static Data get(Rep r, End e)         {
  if (!r->ht[e]) ERROR("empty deque");
  Node tmp = r->ht[e];
  Data d = tmp->data;
  r->ht[e] = tmp->np[!e];
  if (r->ht[e]) {
    r->ht[e]->np[e] = NULL;
  }
  free(tmp);
  r->len--;
  return d;
}
static Data rem(Rep r, End e, Data d) {
  Node n = r->ht[e];
  while (n) {
    if (n->data == d) {
      if (n->np[e]) {
        n->np[e]->np[!e] = n->np[!e];
      } else {
        r->ht[e] = n->np[!e];
      }
      
      free(n);
      r->len--;
      return d;
    }
    n = n->np[!e];
  }
  return 0;
}

extern Deq deq_new() {
  Rep r=(Rep)malloc(sizeof(*r));
  if (!r) ERROR("malloc() failed");
  r->ht[Head]=0;
  r->ht[Tail]=0;
  r->len=0;
  return r;
}

extern int deq_len(Deq q) { return rep(q)->len; }

extern void deq_head_put(Deq q, Data d) {        put(rep(q),Head,d); }
extern Data deq_head_get(Deq q)         { return get(rep(q),Head);   }
extern Data deq_head_ith(Deq q, int i)  { return ith(rep(q),Head,i); }
extern Data deq_head_rem(Deq q, Data d) { return rem(rep(q),Head,d); }

extern void deq_tail_put(Deq q, Data d) {        put(rep(q),Tail,d); }
extern Data deq_tail_get(Deq q)         { return get(rep(q),Tail);   }
extern Data deq_tail_ith(Deq q, int i)  { return ith(rep(q),Tail,i); }
extern Data deq_tail_rem(Deq q, Data d) { return rem(rep(q),Tail,d); }

extern void deq_map(Deq q, DeqMapF f) {
  for (Node n=rep(q)->ht[Head]; n; n=n->np[Tail])
    f(n->data);
}

extern void deq_del(Deq q, DeqMapF f) {
  if (f) deq_map(q,f);
  Node curr=rep(q)->ht[Head];
  while (curr) {
    Node next=curr->np[Tail];
    free(curr);
    curr=next;
  }
  free(q);
}

extern Str deq_str(Deq q, DeqStrF f) {
  char *s=strdup("");
  for (Node n=rep(q)->ht[Head]; n; n=n->np[Tail]) {
    char *d=f ? f(n->data) : n->data;
    char *t; asprintf(&t,"%s%s%s",s,(*s ? " " : ""),d);
    free(s); s=t;
    if (f) free(d);
  }
  return s;
}
