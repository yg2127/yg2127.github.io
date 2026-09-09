#include <assert.h>
#include <stdio.h>

static int exact_search(const int values[], int length, int key) {
    int left = 0;
    int right = length - 1;
    while (left <= right) {
        int middle = left + (right - left) / 2;
        if (values[middle] == key) return middle;
        if (values[middle] < key) left = middle + 1;
        else right = middle - 1;
    }
    return -1;
}

static int predecessor(const int values[], int length, int key) {
    int left = 0;
    int right = length - 1;
    while (left <= right) {
        int middle = left + (right - left) / 2;
        if (values[middle] <= key) left = middle + 1;
        else right = middle - 1;
    }
    return right;
}

static int lower_bound(const int values[], int length, int key) {
    int left = 0;
    int right = length;
    while (left < right) {
        int middle = left + (right - left) / 2;
        if (values[middle] < key) left = middle + 1;
        else right = middle;
    }
    return left;
}

static int linear_predecessor(const int values[], int length, int key) {
    int answer = -1;
    for (int i = 0; i < length; i++) if (values[i] <= key) answer = i;
    return answer;
}

static int linear_lower_bound(const int values[], int length, int key) {
    int index = 0;
    while (index < length && values[index] < key) index++;
    return index;
}

static void check_array(const int values[], int length) {
    for (int key = -2; key <= 5; key++) {
        int exact = exact_search(values, length, key);
        assert(exact == -1 || values[exact] == key);
        assert(predecessor(values, length, key) ==
               linear_predecessor(values, length, key));
        assert(lower_bound(values, length, key) ==
               linear_lower_bound(values, length, key));
    }
}

int main(void) {
    int one[] = {1};
    int duplicates[] = {1, 3, 3, 7};
    int all_equal[] = {2, 2, 2};
    check_array(NULL, 0);
    check_array(one, 1);
    check_array(duplicates, 4);
    check_array(all_equal, 3);
    assert(lower_bound(duplicates, 4, 3) == 1);
    assert(lower_bound(duplicates, 4, 8) == 4);
    puts("binary search contracts against linear references: ok");
}
