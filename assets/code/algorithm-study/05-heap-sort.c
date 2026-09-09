#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void down_heap(int values[], int heap_size, int parent) {
    while (2 * parent <= heap_size) {
        int child = 2 * parent;
        if (child + 1 <= heap_size && values[child + 1] > values[child]) {
            child++;
        }
        if (values[parent] >= values[child]) break;
        swap(&values[parent], &values[child]);
        parent = child;
    }
}

static void heap_sort(int values[], int length) {
    for (int parent = length / 2; parent >= 1; parent--) {
        down_heap(values, length, parent);
    }
    for (int heap_size = length; heap_size >= 2; heap_size--) {
        swap(&values[1], &values[heap_size]);
        down_heap(values, heap_size - 1, 1);
    }
}

static int compare_int(const void *left, const void *right) {
    int a = *(const int *)left;
    int b = *(const int *)right;
    return (a > b) - (a < b);
}

static void check_case(const int input[], int length) {
    int heap_values[16] = {0};
    int expected[15] = {0};
    for (int i = 0; i < length; i++) {
        heap_values[i + 1] = input[i];
        expected[i] = input[i];
    }
    heap_sort(heap_values, length);
    qsort(expected, (size_t)length, sizeof expected[0], compare_int);
    for (int i = 0; i < length; i++) assert(heap_values[i + 1] == expected[i]);
}

int main(void) {
    int one[] = {7};
    int duplicates[] = {8, 3, 8, 1, 6, 2};
    int reversed[] = {5, 4, 3, 2, 1};
    check_case(NULL, 0);
    check_case(one, 1);
    check_case(duplicates, 6);
    check_case(reversed, 5);
    puts("heap sort cases: ok");
}
