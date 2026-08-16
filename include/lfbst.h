#ifndef LIBFLINT_H_BST
#define LIBFLINT_H_BST

#include "lfbinarytree.h"

/*
 * Ordered binary search tree on top of the BinTree node kit.
 * cmp is qsort-style: <0 if a < b, 0 if equal, >0 if a > b.
 * Insert uses bintree_ins_left / bintree_ins_right. Remove splices
 * a single node; it does not prune a subtree.
 */
typedef struct {
    BinTree tree;
    int (*cmp)(const void *a, const void *b);
} Bst;

#define bst_size(t) ((t)->tree.size)
#define bst_root(t) ((t)->tree.root)

int bst_init(Bst *tree, int (*cmp)(const void *a, const void *b),
             void (*destroy)(void *data));

void bst_destroy(Bst *tree);

/* Returns 0 on insert, 1 if data is a duplicate, -1 on error */
int bst_insert(Bst *tree, void *data);

void *bst_find(const Bst *tree, const void *key);

/* Transfers the payload through *data. Does not call destroy. */
int bst_remove(Bst *tree, void **data);

void *bst_min(const Bst *tree);

void *bst_max(const Bst *tree);

#endif /* LIBFLINT_H_BST */
