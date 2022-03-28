#ifndef LIBFLINT_BINARY_TREE_H
#define LIBFLINT_BINARY_TREE_H

typedef struct BinTreeNode {
    void *data;
    struct BinTreeNode *left;
    struct BinTreeNode *right;
} BinTreeNode;

typedef struct {
    int size;

    int (*compare)(const void *a, const void *b);

    void (*destroy)(void *data);

    struct BinTreeNode *root;
} BinTree;

void bintree_init(BinTree *tree, void (*destroy)(void *data));

void bintree_destroy(BinTree *tree);

int bintree_ins_left(BinTree *tree, BinTreeNode *node, void *data);

int bintree_ins_right(BinTree *tree, BinTreeNode *node, void *data);

void bintree_rem_left(BinTree *tree, BinTreeNode *node);

void bintree_rem_right(BinTree *tree, BinTreeNode *node);

int bintree_merge(BinTree *merge, BinTree *left, BinTree *right, void *data);

void bintree_debug_print(BinTree *tree);

#define bintree_is_eob(node) ((node) == NULL)
#define bintree_is_leaf(node) ((node)->left == NULL && (node)->right == NULL)

#endif
