#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void down_min_heap(int heap[], int heap_size, int parent) {
    while (2 * parent + 1 < heap_size) {
        int child = 2 * parent + 1;
        if (child + 1 < heap_size && heap[child + 1] < heap[child]) child++;
        if (heap[parent] <= heap[child]) break;
        swap(&heap[parent], &heap[child]);
        parent = child;
    }
}

static int kth_smallest(const int values[], int length, int k) {
    assert(length > 0 && 1 <= k && k <= length);
    int *heap = malloc((size_t)length * sizeof *heap);
    assert(heap);
    for (int i = 0; i < length; i++) heap[i] = values[i];
    for (int parent = (length - 2) / 2; parent >= 0; parent--) {
        down_min_heap(heap, length, parent);
    }
    int heap_size = length;
    for (int removed = 1; removed < k; removed++) {
        heap[0] = heap[--heap_size];
        down_min_heap(heap, heap_size, 0);
    }
    int answer = heap[0];
    free(heap);
    return answer;
}

static int compare_int(const void *left, const void *right) {
    int a = *(const int *)left;
    int b = *(const int *)right;
    return (a > b) - (a < b);
}

int main(void) {
    int values[] = {7, 2, 9, 2, 5, 1};
    int expected[6];
    for (int i = 0; i < 6; i++) expected[i] = values[i];
    qsort(expected, 6, sizeof expected[0], compare_int);
    for (int k = 1; k <= 6; k++) {
        assert(kth_smallest(values, 6, k) == expected[k - 1]);
    }
    puts("k=1..6 against qsort: ok");
}
