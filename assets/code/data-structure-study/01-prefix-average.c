#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static long slow(const int *a, int n, double *average) {
  long additions = 0;
  for (int i = 0; i < n; i++) {
    long sum = 0;
    for (int j = 0; j <= i; j++) {
      sum += a[j];
      additions++;
    }
    average[i] = (double)sum / (i + 1);
  }
  return additions;
}
static long fast(const int *a, int n, double *average) {
  long additions = 0, sum = 0;
  for (int i = 0; i < n; i++) {
    sum += a[i];
    additions++;
    average[i] = (double)sum / (i + 1);
  }
  return additions;
}
int main(void) {
  for (int n = 4; n <= 32; n *= 2) {
    int *a = malloc((size_t)n * sizeof(*a));
    double *x = malloc((size_t)n * sizeof(*x)),
           *y = malloc((size_t)n * sizeof(*y));
    if (!a || !x || !y) {
      free(a);
      free(x);
      free(y);
      return 1;
    }
    for (int i = 0; i < n; i++)
      a[i] = i % 7 - 3;
    long slow_adds = slow(a, n, x), fast_adds = fast(a, n, y);
    for (int i = 0; i < n; i++)
      assert(fabs(x[i] - y[i]) < 1e-12);
    printf("n=%d slow_adds=%ld fast_adds=%ld averages_equal=true\n", n,
           slow_adds, fast_adds);
    free(a);
    free(x);
    free(y);
  }
}
