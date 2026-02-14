#include <stdio.h>
#include <stdlib.h>
#include "lftest.h"
#include "lfbinarytree.h"

int main() {
    BinTree *tree = malloc(sizeof(BinTree));
    bintree_init(tree, NULL);

    int root = 0;
    int l1 = 1;
    int l2 = 2;
    int r1 = 12;
    int r2 = 200;

    bintree_ins_left(tree, NULL, &root);
    ASSERT_NOT_NULL(tree->root);

    bintree_ins_left(tree, tree->root, &l1);
    bintree_ins_left(tree, tree->root->left, &l2);
    bintree_ins_right(tree, tree->root->left, &r2);
    bintree_ins_right(tree, tree->root, &r1);
    bintree_ins_right(tree, tree->root->right, &r2);
    bintree_ins_left(tree, tree->root->right, &l1);

    ASSERT_EQ(*(int *)tree->root->data, 0);
    ASSERT_EQ(*(int *)tree->root->left->data, 1);
    ASSERT_EQ(*(int *)tree->root->right->data, 12);

    bintree_destroy(tree);
    free(tree);

    TEST_REPORT();
}
