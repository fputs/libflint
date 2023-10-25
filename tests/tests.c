#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "linkedlist.h"
#include "set.h"
#include "stack.h"
#include "binarytree.h"
#include "vector.h"
#include "math.h"

void print_ll(List *list) {
    LL_ITER(list) {
        printf(" %d", *((int *) node->data));
    }
    printf("\n");
}

void test_ll() {
    printf("\n--- LIST TEST ---\n");
    List *list = malloc(sizeof(List));
    ll_init(list, NULL);

    int i = 1;
    int j = 2;
    int k = 4;

    ll_ins_next(list, list->head, (void *) &i);
    ll_ins_next(list, list->tail, (void *) &j);
    ll_ins_next(list, list->tail, (void *) &k);

    printf("List: ");
    print_ll(list);

    void *data;
    ll_remove_next(list, list->head, &data);

    printf("List: ");
    print_ll(list);
    printf("Removed: %d\n", *((int *) data));

    ll_destroy(list);
    free(list);
}

int int_match(const void *a, const void *b) {
    return *((int *) a) == *((int *) b);
}

void test_set() {
    printf("\n--- SET TEST ---\n");
    Set *set = malloc(sizeof(Set));
    set_init(set, int_match, NULL);

    int i = 1;
    int j = 2;
    int k = 2;

    set_insert(set, (void *) &i);
    set_insert(set, (void *) &j);
    set_insert(set, (void *) &k);

    int i2 = 1;
    int j2 = 4;

    Set *set2 = malloc(sizeof(Set));
    set_init(set2, int_match, NULL);

    set_insert(set2, (void *) &i2);
    set_insert(set2, (void *) &j2);

    printf("Set 1:");
    print_ll(set);

    printf("Set 2:");
    print_ll(set2);
    printf("\n");

    Set *set_u = malloc(sizeof(Set));
    Set *set_i = malloc(sizeof(Set));
    Set *set_d = malloc(sizeof(Set));

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
    Stack *stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    int a = 1, b = 2;
    stack_push(stack, &a);
    stack_push(stack, &b);
    printf("Stack size: %lu\n", stack->size);

    int *p = NULL;
    stack_pop(stack, (void **) &p);
    printf("b = %d\n", *p);

    stack_pop(stack, (void **) &p);
    printf("a = %d\n", *p);
    printf("Stack size: %lu\n", stack->size);

    stack_destroy(stack);
    free(stack);
    stack = NULL;
}

void test_bintree() {
    printf("\n--- BINARY TREE TEST ---\n");
    BinTree *tree = malloc(sizeof(BinTree));
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

    printf("Changing r2\n");
    r2 = 100;
    bintree_debug_print(tree);

    bintree_destroy(tree);
    free(tree);
    tree = NULL;
}

void test_math() {
    printf("\n--- MATH TEST ---\n");
    int i = 1, j = 2;
    assert(max_int(i, j) == j);
    printf("Between %d and %d, %d is larger\n", i, j, max_int(i, j));
    printf("Between %d and %d, %d is smaller\n", i, j, min_int(i, j));

    char *s = "10101101";
    printf("Binary: %s\n", s);
    printf("Decimal: %d\n", binstr_to_int(s));

    char *s2 = "1010_1101";
    printf("Binary: %s\n", s2);
    printf("Decimal: %d\n", binstr_to_int(s2));

    printf("\nGenerate line from 0,0 to 2,5\n");
    size_t sz = 0;
    Point *line = bresenham(0, 0, 2, 5, &sz);
    for (size_t idx = 0; idx < sz; idx++) {
        printf("%d,%d ", line[idx].x, line[idx].y);
    }
    printf("\n");
    free(line);
}

void print_vector(Vector *vec) {
    for (size_t i = 0; i < vec->length; ++i) {
        int t = *(int*)vec_at(vec, i);
        printf("%d ", t);
    }
    printf("\n");
}

void test_vector() {
    printf("\n--- VECTOR TEST ---\n");
    Vector *v = malloc(sizeof(Vector));
    vec_init(v, NULL);

    int e0 = 0;
    int e1 = 1;
    int e2 = 2;
    int e3 = 3;
    int e4 = 4;

    vec_push(v, &e0);
    assert(v->length == 1);
    int *t = (int*)vec_at(v, 0);
    assert(*t == 0);

    vec_push(v, &e1);
    vec_push(v, &e2);
    assert(v->length == 3);

    // test access outside bounds
    t = (int*)vec_safe_at(v, 3);
    assert(t == NULL);

    printf("Before insert: ");
    print_vector(v);
    vec_push(v, &e4);
    vec_insert(v, &e3, 3);
    printf("After insert:  ");
    print_vector(v);

    t = (int*)vec_at(v, 3);
    assert(*t == e3);
    t = (int*)vec_at(v, 4);
    assert(*t == e4);

    t = (int*)vec_remove(v, 1);
    assert(t != NULL);
    assert(*t == 1);
    printf("After removal: ");
    print_vector(v);

    t = (int*)vec_remove(v, 10);
    assert(t == NULL);

    vec_destroy(v);
}

int main() {
    test_ll();
    test_set();
    test_stack();
    test_bintree();
    test_math();
    test_vector();
    return 0;
}