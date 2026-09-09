#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void selection_sort(int values[], int length) {
    for (int i = 0; i < length - 1; i++) {
        int minimum = i;
        for (int j = i + 1; j < length; j++) {
            if (values[j] < values[minimum]) minimum = j;
        }
        swap(&values[i], &values[minimum]);
    }
}

static void insertion_sort(int values[], int length) {
    for (int i = 1; i < length; i++) {
        int key = values[i];
        int j = i - 1;
        while (j >= 0 && values[j] > key) {
            values[j + 1] = values[j];
            j--;
        }
        values[j + 1] = key;
    }
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
            int selection[6];
            int insertion[6];
            int expected[6];
            int digits = code;
            for (int i = 0; i < length; i++) {
                selection[i] = insertion[i] = expected[i] = digits % 3 - 1;
                digits /= 3;
            }
            selection_sort(selection, length);
            insertion_sort(insertion, length);
            qsort(expected, (size_t)length, sizeof expected[0], compare_int);
            for (int i = 0; i < length; i++) {
                assert(selection[i] == expected[i]);
                assert(insertion[i] == expected[i]);
            }
            test_count++;
        }
    }
    printf("%d selection/insertion arrays: ok\n", test_count);
}
