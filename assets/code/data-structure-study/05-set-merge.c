#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct Node {
  int value;
  struct Node *next;
} Node;
static int add_sorted(Node *head, int value) {
  Node *p = head;
  while (p->next && p->next->value < value)
    p = p->next;
  if (p->next && p->next->value == value)
    return 1;
  Node *n = malloc(sizeof(*n));
  if (!n)
    return 0;
  *n = (Node){value, p->next};
  p->next = n;
  return 1;
}
static int append(Node **tail, int value) {
  Node *n = malloc(sizeof(*n));
  if (!n)
    return 0;
  *n = (Node){value, NULL};
  (*tail)->next = n;
  *tail = n;
  return 1;
}
static void clear(Node *h) {
  while (h) {
    Node *n = h->next;
    free(h);
    h = n;
  }
}
static Node *combine(Node *a, Node *b, int intersection) {
  Node *r = calloc(1, sizeof(*r));
  if (!r)
    return NULL;
  Node *tail = r, *p = a->next, *q = b->next;
  while (p || q) {
    int emit = 0, value = 0;
    if (!q || (p && p->value < q->value)) {
      value = p->value;
      emit = !intersection;
      p = p->next;
    } else if (!p || q->value < p->value) {
      value = q->value;
      emit = !intersection;
      q = q->next;
    } else {
      value = p->value;
      emit = 1;
      p = p->next;
      q = q->next;
    }
    if (emit && !append(&tail, value)) {
      clear(r);
      return NULL;
    }
  }
  return r;
}
static int equals(Node *h, const int *v, int n) {
  Node *p = h->next;
  for (int i = 0; i < n; i++, p = p->next)
    if (!p || p->value != v[i])
      return 0;
  return p == NULL;
}
static void show(const char *name, Node *h) {
  printf("%s", name);
  for (Node *p = h->next; p; p = p->next)
    printf(" %d", p->value);
  puts("");
}
int main(void) {
  Node *a = calloc(1, sizeof(*a)), *b = calloc(1, sizeof(*b));
  if (!a || !b) {
    clear(a);
    clear(b);
    return 1;
  }
  int av[] = {3, 1, 3, 2}, bv[] = {4, 3, 2};
  for (int i = 0; i < 4; i++)
    assert(add_sorted(a, av[i]));
  for (int i = 0; i < 3; i++)
    assert(add_sorted(b, bv[i]));
  Node *u = combine(a, b, 0), *in = combine(a, b, 1);
  assert(u && in);
  int uv[] = {1, 2, 3, 4}, iv[] = {2, 3};
  assert(equals(u, uv, 4) && equals(in, iv, 2));
  Node *empty = calloc(1, sizeof(*empty));
  Node *empty_i = empty ? combine(a, empty, 1) : NULL;
  assert(empty_i && empty_i->next == NULL);
  show("A:", a);
  show("union:", u);
  show("intersection:", in);
  puts("empty intersection: empty");
  clear(a);
  clear(b);
  clear(u);
  clear(in);
  clear(empty);
  clear(empty_i);
}
