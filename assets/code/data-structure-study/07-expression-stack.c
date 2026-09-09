#include <stddef.h>
#include <stdio.h>
static int priority(char c) {
  return (c == '*' || c == '/') ? 2 : (c == '+' || c == '-') ? 1 : 0;
}
static int to_postfix(const char *in, char *out, size_t cap) {
  char ops[32];
  int top = -1;
  size_t n = 0;
  for (size_t i = 0; in[i]; i++) {
    char c = in[i];
    if (c >= '0' && c <= '9') {
      if (n + 1 >= cap)
        return 0;
      out[n++] = c;
    } else if (c == '(') {
      if (top + 1 >= (int)sizeof(ops))
        return 0;
      ops[++top] = c;
    } else if (c == ')') {
      while (top >= 0 && ops[top] != '(')
        out[n++] = ops[top--];
      if (top < 0)
        return 0;
      top--;
    } else if (priority(c)) {
      while (top >= 0 && ops[top] != '(' && priority(ops[top]) >= priority(c))
        out[n++] = ops[top--];
      ops[++top] = c;
    } else
      return 0;
    if (n + 1 >= cap)
      return 0;
  }
  while (top >= 0) {
    if (ops[top] == '(')
      return 0;
    out[n++] = ops[top--];
  }
  out[n] = '\0';
  return 1;
}
static int evaluate(const char *s, int *out) {
  int st[32], top = -1;
  for (int i = 0; s[i]; i++) {
    char c = s[i];
    if (c >= '0' && c <= '9')
      st[++top] = c - '0';
    else {
      if (top < 1)
        return 0;
      int r = st[top--], l = st[top--];
      if (c == '/' && r == 0)
        return 0;
      st[++top] = c == '+'   ? l + r
                  : c == '-' ? l - r
                  : c == '*' ? l * r
                             : l / r;
    }
  }
  if (top != 0)
    return 0;
  *out = st[0];
  return 1;
}
int main(void) {
  const char *e[] = {"8-2-1", "8-(2-1)", "2+3*4"};
  for (int i = 0; i < 3; i++) {
    char post[32];
    int value;
    if (!to_postfix(e[i], post, sizeof(post)) || !evaluate(post, &value))
      return 1;
    printf("%s -> %s -> %d\n", e[i], post, value);
  }
  char post[32];
  printf("invalid brackets rejected: %s\n",
         to_postfix("]( ", post, sizeof(post)) ? "false" : "true");
}
