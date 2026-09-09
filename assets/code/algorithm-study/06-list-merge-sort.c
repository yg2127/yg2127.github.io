#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    int original_index;
    struct Node *next;
} Node;

static Node *merge(Node *left, Node *right) {
    Node dummy = {0, 0, NULL};
    Node *tail = &dummy;
    while (left && right) {
        if (left->key <= right->key) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    tail->next = left ? left : right;
    return dummy.next;
}

static Node *merge_sort(Node *head) {
    if (!head || !head->next) return head;
    Node *slow = head;
    Node *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node *right = slow->next;
    slow->next = NULL;
    return merge(merge_sort(head), merge_sort(right));
}

static Node *make_list(const int values[], int length) {
    Node *head = NULL;
    Node **next_slot = &head;
    for (int i = 0; i < length; i++) {
        *next_slot = malloc(sizeof **next_slot);
        assert(*next_slot);
        **next_slot = (Node){values[i], i, NULL};
        next_slot = &(*next_slot)->next;
    }
    return head;
}

static void free_list(Node *head) {
    while (head) {
        Node *next = head->next;
        free(head);
        head = next;
    }
}

static void check_case(const int values[], int length) {
    Node *head = merge_sort(make_list(values, length));
    int previous_key = 0;
    int previous_index = -1;
    int seen = 0;
    for (Node *node = head; node; node = node->next) {
        if (seen) assert(previous_key <= node->key);
        if (seen && previous_key == node->key) assert(previous_index < node->original_index);
        previous_key = node->key;
        previous_index = node->original_index;
        seen++;
    }
    assert(seen == length);
    free_list(head);
}

int main(void) {
    int one[] = {4};
    int even[] = {3, 1, 3, 2};
    int odd[] = {5, 2, 5, 1, 5};
    check_case(NULL, 0);
    check_case(one, 1);
    check_case(even, 4);
    check_case(odd, 5);
    puts("stable list merge sort cases: ok");
}
