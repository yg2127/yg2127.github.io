#include <stdio.h>
#include <stdlib.h>
typedef struct Node {
  int value;
  struct Node *next;
} Node;
typedef struct {
  Node *front, *rear;
  size_t size;
} Queue;
static int enqueue(Queue *q, int v) {
  Node *n = malloc(sizeof *n);
  if (!n)
    return 0;
  *n = (Node){v, NULL};
  if (q->rear)
    q->rear->next = n;
  else
    q->front = n;
  q->rear = n;
  q->size++;
  return 1;
}
static int dequeue(Queue *q, int *out) {
  if (!q->front)
    return 0;
  Node *n = q->front;
  *out = n->value;
  q->front = n->next;
  free(n);
  q->size--;
  if (!q->front)
    q->rear = NULL;
  return 1;
}
static void state(const char *name, Queue *q) {
  printf("%s size=%zu front=%s rear=%s\n", name, q->size,
         q->front ? "node" : "NULL", q->rear ? "node" : "NULL");
}
int main(void) {
  Queue q = {0};
  int v;
  state("empty", &q);
  if (!enqueue(&q, 10))
    return 1;
  state("one", &q);
  dequeue(&q, &v);
  printf("removed=%d ", v);
  state("empty-again", &q);
  if (!enqueue(&q, 20))
    return 1;
  dequeue(&q, &v);
  printf("reused=%d\n", v);
}
