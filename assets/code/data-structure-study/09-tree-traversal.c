#include <stdio.h>
typedef struct Node {
  int v;
  struct Node *l, *r;
} Node;
static void pre(Node *n) {
  if (!n)
    return;
  printf("%d", n->v);
  pre(n->l);
  pre(n->r);
}
static void in(Node *n) {
  if (!n)
    return;
  in(n->l);
  printf("%d", n->v);
  in(n->r);
}
static void post(Node *n) {
  if (!n)
    return;
  post(n->l);
  post(n->r);
  printf("%d", n->v);
}
static int sum(Node *n) { return n ? sum(n->l) + sum(n->r) + n->v : 0; }
int main(void) {
  Node d = {4, NULL, NULL}, e = {5, NULL, NULL}, b = {2, &d, &e},
       c = {3, NULL, NULL}, a = {1, &b, &c};
  printf("pre ");
  pre(&a);
  printf("\nin  ");
  in(&a);
  printf("\npost ");
  post(&a);
  printf("\nsum %d\n", sum(&a));
}
