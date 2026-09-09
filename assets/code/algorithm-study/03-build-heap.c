#include <assert.h>
#include <stdio.h>

static long bottom_up_swaps;
static long insertion_swaps;

static void swap_counted(int *a, int *b, long *counter) {
    int temp = *a;
    *a = *b;
    *b = temp;
    (*counter)++;
}

static void down_heap(int values[], int length, int parent) {
    while (2 * parent <= length) {
        int child = 2 * parent;
        if (child + 1 <= length && values[child + 1] > values[child]) child++;
        if (values[parent] >= values[child]) break;
        swap_counted(&values[parent], &values[child], &bottom_up_swaps);
        parent = child;
    }
}

static void build_bottom_up(int values[], int length) {
    for (int parent = length / 2; parent >= 1; parent--) {
        down_heap(values, length, parent);
    }
}

static void build_by_insertion(int result[], const int input[], int length) {
    for (int size = 1; size <= length; size++) {
        result[size] = input[size];
        int child = size;
        while (child > 1 && result[child] > result[child / 2]) {
            swap_counted(&result[child], &result[child / 2], &insertion_swaps);
            child /= 2;
        }
    }
}

static void check_heap(const int values[], int length) {
    for (int child = 2; child <= length; child++) {
        assert(values[child / 2] >= values[child]);
    }
}

int main(void) {
    int input[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    int bottom_up[16];
    int insertion[16];
    for (int i = 1; i <= 15; i++) bottom_up[i] = input[i];
    build_bottom_up(bottom_up, 15);
    build_by_insertion(insertion, input, 15);
    check_heap(bottom_up, 15);
    check_heap(insertion, 15);
    printf("bottom-up swaps=%ld, insertion swaps=%ld\n",
           bottom_up_swaps, insertion_swaps);
}
