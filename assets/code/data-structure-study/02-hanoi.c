#include <stdio.h>

static long moves;
static void hanoi(int n, char from, char via, char to) {
  if (n <= 0)
    return;
  hanoi(n - 1, from, to, via);
  moves++;
  hanoi(n - 1, via, from, to);
}

int main(void) {
  for (int n = 0; n <= 4; n++) {
    moves = 0;
    hanoi(n, 'A', 'B', 'C');
    printf("n=%d moves=%ld\n", n, moves);
  }
}
