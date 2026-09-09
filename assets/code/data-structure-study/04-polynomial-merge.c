#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int coef, exp;
  struct Node *next;
} Node;
static void clear(Node *p);

static int append(Node **tail, int coef, int exp) {
  if (coef == 0)
    return 1;
  (*tail)->next = malloc(sizeof(Node));
  if (!(*tail)->next)
    return 0;
  *tail = (*tail)->next;
  **tail = (Node){coef, exp, NULL};
  return 1;
}

static Node *add(Node *a, Node *b) {
  Node dummy = {0, 0, NULL}, *tail = &dummy;
  while (a && b) {
    int ok;
    if (a->exp > b->exp) {
      ok = append(&tail, a->coef, a->exp);
      a = a->next;
    } else if (a->exp < b->exp) {
      ok = append(&tail, b->coef, b->exp);
      b = b->next;
    } else {
      ok = append(&tail, a->coef + b->coef, a->exp);
      a = a->next;
      b = b->next;
    }
    if (!ok) {
      clear(dummy.next);
      return NULL;
    }
  }
  while (a) {
    if (!append(&tail, a->coef, a->exp)) {
      clear(dummy.next);
      return NULL;
    }
    a = a->next;
  }
  while (b) {
    if (!append(&tail, b->coef, b->exp)) {
      clear(dummy.next);
      return NULL;
    }
    b = b->next;
  }
  return dummy.next;
}

static void clear(Node *p) {
  while (p) {
    Node *next = p->next;
    free(p);
    p = next;
  }
}
int main(void) {
  Node a2 = {2, 0, NULL}, a1 = {3, 4, &a2};
  Node b3 = {-2, 0, NULL}, b2 = {5, 1, &b3}, b1 = {-3, 4, &b2};
  Node *sum = add(&a1, &b1);
  assert(sum && sum->coef == 5 && sum->exp == 1 && sum->next == NULL);
  for (Node *p = sum; p; p = p->next)
    printf("%dx^%d ", p->coef, p->exp);
  putchar('\n');
  clear(sum);
}
