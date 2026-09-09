#include <stdio.h>
#include <stdlib.h>
typedef struct Node {
  char value;
  struct Node *next;
} Node;
typedef struct {
  Node *top;
  size_t size, max;
} Stack;
static int push(Stack *s, char v) {
  if (s->size == s->max)
    return 0;
  Node *n = malloc(sizeof *n);
  if (!n)
    return 0;
  *n = (Node){v, s->top};
  s->top = n;
  s->size++;
  return 1;
}
static int pop(Stack *s, char *out) {
  if (!s->top)
    return 0;
  Node *n = s->top;
  *out = n->value;
  s->top = n->next;
  free(n);
  s->size--;
  return 1;
}
int main(void) {
  Stack s = {NULL, 0, 2};
  char v;
  printf("push A %d\n", push(&s, 'A'));
  printf("push B %d\n", push(&s, 'B'));
  printf("overflow C %d\n", push(&s, 'C'));
  while (pop(&s, &v))
    printf("pop %c size=%zu\n", v, s.size);
  printf("underflow %d top_null=%d\n", pop(&s, &v), s.top == NULL);
}
