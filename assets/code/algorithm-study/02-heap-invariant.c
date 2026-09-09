#include <assert.h>
#include <stdio.h>

#define CAP 64
static int heap[CAP + 1];
static int size;

static void swap(int *a, int *b) { int t = *a; *a = *b; *b = t; }
static void up_heap(int i) {
    while (i > 1 && heap[i] > heap[i / 2]) {
        swap(&heap[i], &heap[i / 2]);
        i /= 2;
    }
}
static void down_heap(int i) {
    while (2 * i <= size) {
        int child = 2 * i;
        if (child + 1 <= size && heap[child + 1] > heap[child]) child++;
        if (heap[i] >= heap[child]) break;
        swap(&heap[i], &heap[child]);
        i = child;
    }
}
static void insert(int value) { assert(size < CAP); heap[++size] = value; up_heap(size); }
static int remove_max(void) {
    assert(size > 0);
    int answer = heap[1];
    heap[1] = heap[size--];
    if (size) down_heap(1);
    return answer;
}
static void check_invariant(void) {
    for (int i = 2; i <= size; i++) assert(heap[i / 2] >= heap[i]);
}
int main(void) {
    int input[] = {4, 9, 1, 7, 7, 12, 3};
    int expected[] = {12, 9, 7, 7, 4, 3, 1};
    for (int i = 0; i < 7; i++) { insert(input[i]); check_invariant(); }
    for (int i = 0; i < 7; i++) { assert(remove_max() == expected[i]); check_invariant(); }
    puts("heap invariant: ok");
}
