#include <stdio.h>
#include <stdlib.h>
typedef struct Node {
  int id, left_id, right_id;
  struct Node *left, *right;
} Node;
static int index_of(Node *a, int n, int id) {
  for (int i = 0; i < n; i++)
    if (a[i].id == id)
      return i;
  return -1;
}
static int visit(Node *a, int i, int *color, int *count) {
  if (color[i] == 1)
    return 0;
  if (color[i] == 2)
    return 1;
  color[i] = 1;
  (*count)++;
  if (a[i].left && !visit(a, (int)(a[i].left - a), color, count))
    return 0;
  if (a[i].right && !visit(a, (int)(a[i].right - a), color, count))
    return 0;
  color[i] = 2;
  return 1;
}
static Node *build(Node *a, int n) {
  int *degree = calloc((size_t)n, sizeof(*degree)),
      *color = calloc((size_t)n, sizeof(*color));
  if (!degree || !color) {
    free(degree);
    free(color);
    return NULL;
  }
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++)
      if (a[i].id == a[j].id)
        goto fail;
    int li = a[i].left_id ? index_of(a, n, a[i].left_id) : -1,
        ri = a[i].right_id ? index_of(a, n, a[i].right_id) : -1;
    if ((a[i].left_id && li < 0) || (a[i].right_id && ri < 0))
      goto fail;
    a[i].left = li >= 0 ? &a[li] : NULL;
    a[i].right = ri >= 0 ? &a[ri] : NULL;
    if (li >= 0 && ++degree[li] > 1)
      goto fail;
    if (ri >= 0 && ++degree[ri] > 1)
      goto fail;
  }
  int root = -1, roots = 0;
  for (int i = 0; i < n; i++)
    if (degree[i] == 0) {
      root = i;
      roots++;
    }
  if (roots != 1)
    goto fail;
  int count = 0;
  if (!visit(a, root, color, &count) || count != n)
    goto fail;
  free(degree);
  free(color);
  return &a[root];
fail:
  free(degree);
  free(color);
  return NULL;
}
static void preorder(Node *n) {
  if (!n)
    return;
  printf("%d ", n->id);
  preorder(n->left);
  preorder(n->right);
}
int main(void) {
  Node rows[] = {{3, 0, 0, NULL, NULL},
                 {1, 2, 3, NULL, NULL},
                 {2, 4, 0, NULL, NULL},
                 {4, 0, 0, NULL, NULL}};
  Node *r = build(rows, 4);
  printf("shuffled preorder: ");
  preorder(r);
  puts("");
  Node missing[] = {{1, 9, 0, NULL, NULL}};
  Node duplicate[] = {{1, 0, 0, NULL, NULL}, {1, 0, 0, NULL, NULL}};
  Node cycle[] = {{1, 2, 0, NULL, NULL}, {2, 1, 0, NULL, NULL}};
  Node roots[] = {{1, 0, 0, NULL, NULL}, {2, 0, 0, NULL, NULL}};
  printf(
      "invalid rejected: missing=%d duplicate=%d cycle=%d multiple_roots=%d\n",
      !build(missing, 1), !build(duplicate, 2), !build(cycle, 2),
      !build(roots, 2));
}
