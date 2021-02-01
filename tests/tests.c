#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"
#include "set.h"
#include "stack.h"
#include "binarytree.h"

void print_ll(struct List* list) {
    for (struct ListNode* node = list->head; node != NULL; node = node->next) {
        printf(" %d", *((int*)node->data));
    }
    printf("\n");
}

void test_ll() {
    printf("\n--- LIST TEST ---\n");
    struct List* list = malloc(sizeof(struct List));
    ll_init(list, NULL);

    int i = 1;
    int j = 2;
    int k = 4;

    ll_ins_next(list, list->head, (void*)&i);
    ll_ins_next(list, list->tail, (void*)&j);
    ll_ins_next(list, list->tail, (void*)&k);

    printf("struct List: ");
    print_ll(list);

    void* data;
    ll_remove_next(list, list->head, &data);

    printf("struct List: ");
    print_ll(list);
    printf("Removed: %d\n", *((int*)data));

    ll_destroy(list);
    free(list);
}

int int_match(const void* a, const void* b) {
    return *((int*)a) == *((int*)b);
}

void test_set() {
    printf("\n--- SET TEST ---\n");
    struct Set* set = malloc(sizeof(struct Set));
    set_init(set, int_match, NULL);

    int i = 1;
    int j = 2;
    int k = 2;

    set_insert(set, (void*)&i);
    set_insert(set, (void*)&j);
    set_insert(set, (void*)&k);

    int i2 = 1;
    int j2 = 4;

    struct Set* set2 = malloc(sizeof(struct Set));
    set_init(set2, int_match, NULL);

    set_insert(set2, (void*)&i2);
    set_insert(set2, (void*)&j2);

    printf("struct Set 1:");
    print_ll(set);

    printf("struct Set 2:");
    print_ll(set2);
    printf("\n");

    struct Set* set_u = malloc(sizeof(struct Set));
    struct Set* set_i = malloc(sizeof(struct Set));
    struct Set* set_d = malloc(sizeof(struct Set));

    set_union(set_u, set, set2);
    printf("Union:");
    print_ll(set_u);

    set_difference(set_d, set, set2);
    printf("Difference:");
    print_ll(set_d);

    set_intersection(set_i, set, set2);
    printf("Intersection:");
    print_ll(set_i);

    set_destroy(set);
    set_destroy(set2);
    set_destroy(set_u);
    set_destroy(set_i);
    set_destroy(set_d);
    free(set);
    free(set2);
    free(set_u);
    free(set_i);
    free(set_d);
}

void test_stack() {
    printf("\n--- STACK TEST ---\n");
    struct Stack *stack = malloc(sizeof(struct Stack));
    stack_init(stack, NULL);

    int a = 1, b = 2;
    stack_push(stack, &a);
    stack_push(stack, &b);
    printf("struct Stack size: %lu\n", stack->size);

    int *p = NULL;
    stack_pop(stack, (void **)&p);
    printf("b = %d\n", *p);

    stack_pop(stack, (void **)&p);
    printf("a = %d\n", *p);
    printf("struct Stack size: %lu\n", stack->size);

    stack_destroy(stack);
}

void test_bintree() {
    printf("\n--- BINARY TREE TEST ---\n");
    struct BinTree *tree = malloc(sizeof(struct BinTree));
    bintree_init(tree, NULL);

    int root = 0;
    int l1 = 1;
    int l2 = 2;
    int r1 = 12;
    int r2 = 200;

    bintree_ins_left(tree, NULL, &root);
    bintree_ins_left(tree, tree->root, &l1);
    bintree_ins_left(tree, tree->root->left, &l2);
    bintree_ins_right(tree, tree->root->left, &r2);
    bintree_ins_right(tree, tree->root, &r1);
    bintree_ins_right(tree, tree->root->right, &r2);
    bintree_ins_left(tree, tree->root->right, &l1);

    bintree_debug_print(tree);

    bintree_destroy(tree);
}

int main() {
    test_ll();
    test_set();
    test_stack();
    test_bintree();
    return 0;
}