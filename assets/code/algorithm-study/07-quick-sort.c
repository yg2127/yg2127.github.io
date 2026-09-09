#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void quick_sort(int values[], int left, int right) {
    if (left >= right) return;
    int pivot = values[left + (right - left) / 2];
    int less_end = left;
    int scan = left;
    int greater_start = right;
    while (scan <= greater_start) {
        if (values[scan] < pivot) {
            swap(&values[scan], &values[less_end]);
            scan++;
            less_end++;
        } else if (values[scan] > pivot) {
            swap(&values[scan], &values[greater_start]);
            greater_start--;
        } else {
            scan++;
        }
    }
    quick_sort(values, left, less_end - 1);
    quick_sort(values, greater_start + 1, right);
}

static int compare_int(const void *left, const void *right) {
    int a = *(const int *)left;
    int b = *(const int *)right;
    return (a > b) - (a < b);
}

int main(void) {
    int test_count = 0;
    for (int length = 0, combinations = 1;
         length <= 6;
         length++, combinations *= 3) {
        for (int code = 0; code < combinations; code++) {
            int actual[6];
            int expected[6];
            int digits = code;
            for (int i = 0; i < length; i++) {
                actual[i] = expected[i] = digits % 3;
                digits /= 3;
            }
            quick_sort(actual, 0, length - 1);
            qsort(expected, (size_t)length, sizeof expected[0], compare_int);
            for (int i = 0; i < length; i++) assert(actual[i] == expected[i]);
            test_count++;
        }
    }
    printf("%d quick sort arrays: ok\n", test_count);
}
