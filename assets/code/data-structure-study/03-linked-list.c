#include <stdio.h>
typedef struct Node {
  char value;
  struct Node *prev, *next;
} Node;
static void bad(Node *curr, Node *n) {
  n->next = curr;
  n->prev = curr->prev;
  curr->prev = n;
  curr->prev->next = n;
}
static void good(Node *curr, Node *n) {
  n->next = curr;
  n->prev = curr->prev;
  curr->prev->next = n;
  curr->prev = n;
}
int main(void) {
  Node a = {'A', NULL, NULL}, b = {'B', &a, NULL}, x = {'X', NULL, NULL};
  a.next = &b;
  bad(&b, &x);
  printf("bad self=%d left_connected=%d\n", x.next == &x, a.next == &x);
  Node h = {'H', NULL, NULL}, t = {'T', &h, NULL}, m = {'M', NULL, NULL};
  h.next = &t;
  good(&t, &m);
  printf("good links=%d\n",
         h.next == &m && m.prev == &h && m.next == &t && t.prev == &m);
}
