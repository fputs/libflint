#ifndef LIBFLINT_BINARY_TREE_H
#define LIBFLINT_BINARY_TREE_H

struct BinTreeNode {
    void *data;
    struct BinTreeNode *left;
    struct BinTreeNode *right;
};

struct BinTree {
    int size;

    int (*compare)(const void *a, const void *b);
    void (*destroy)(void *data);

    struct BinTreeNode *root;
};

void bintree_init(struct BinTree *tree, void (*destroy)(void *data));
void bintree_destroy(struct BinTree *tree);
int bintree_ins_left(struct BinTree *tree, struct BinTreeNode *node, void *data);
int bintree_ins_right(struct BinTree *tree, struct BinTreeNode *node, void *data);
void bintree_rem_left(struct BinTree *tree, struct BinTreeNode *node);
void bintree_rem_right(struct BinTree *tree, struct BinTreeNode *node);
int bintree_merge(struct BinTree *merge, struct BinTree *left, struct BinTree *right, void *data);
void bintree_debug_print(struct BinTree *tree);

#define bintree_is_eob(node) ((node) == NULL)
#define bintree_is_leaf(node) ((node)->left == NULL && (node)->right == NULL)

#endif
