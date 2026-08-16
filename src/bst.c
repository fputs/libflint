#include <stdlib.h>

#include "lfbst.h"

int bst_init(Bst *tree, int (*cmp)(const void *a, const void *b),
             void (*destroy)(void *data)) {
    if (tree == NULL || cmp == NULL) {
        return -1;
    }
    bintree_init(&tree->tree, destroy);
    tree->cmp = cmp;
    return 0;
}

void bst_destroy(Bst *tree) {
    if (tree == NULL) {
        return;
    }
    bintree_destroy(&tree->tree);
    tree->cmp = NULL;
}

int bst_insert(Bst *tree, void *data) {
    if (tree == NULL || tree->cmp == NULL) {
        return -1;
    }

    if (tree->tree.root == NULL) {
        return bintree_ins_left(&tree->tree, NULL, data);
    }

    BinTreeNode *node = tree->tree.root;
    for (;;) {
        int c = tree->cmp(data, node->data);
        if (c == 0) {
            return 1;
        }
        if (c < 0) {
            if (node->left == NULL) {
                return bintree_ins_left(&tree->tree, node, data);
            }
            node = node->left;
        } else {
            if (node->right == NULL) {
                return bintree_ins_right(&tree->tree, node, data);
            }
            node = node->right;
        }
    }
}

void *bst_find(const Bst *tree, const void *key) {
    if (tree == NULL || tree->cmp == NULL) {
        return NULL;
    }

    BinTreeNode *node = tree->tree.root;
    while (node != NULL) {
        int c = tree->cmp(key, node->data);
        if (c == 0) {
            return node->data;
        }
        node = c < 0 ? node->left : node->right;
    }
    return NULL;
}

int bst_remove(Bst *tree, void **data) {
    if (tree == NULL || data == NULL || tree->cmp == NULL) {
        return -1;
    }

    BinTreeNode *parent = NULL;
    BinTreeNode *node = tree->tree.root;
    while (node != NULL) {
        int c = tree->cmp(*data, node->data);
        if (c == 0) {
            break;
        }
        parent = node;
        node = c < 0 ? node->left : node->right;
    }
    if (node == NULL) {
        return -1;
    }

    void *payload = node->data;

    if (node->left != NULL && node->right != NULL) {
        BinTreeNode *sp = node;
        BinTreeNode *succ = node->right;
        while (succ->left != NULL) {
            sp = succ;
            succ = succ->left;
        }
        node->data = succ->data;
        BinTreeNode *child = succ->right;
        if (sp->left == succ) {
            sp->left = child;
        } else {
            sp->right = child;
        }
        free(succ);
    } else {
        BinTreeNode *child = node->left != NULL ? node->left : node->right;
        if (parent == NULL) {
            tree->tree.root = child;
        } else if (parent->left == node) {
            parent->left = child;
        } else {
            parent->right = child;
        }
        free(node);
    }

    tree->tree.size--;
    *data = payload;
    return 0;
}

void *bst_min(const Bst *tree) {
    if (tree == NULL || tree->tree.root == NULL) {
        return NULL;
    }
    BinTreeNode *node = tree->tree.root;
    while (node->left != NULL) {
        node = node->left;
    }
    return node->data;
}

void *bst_max(const Bst *tree) {
    if (tree == NULL || tree->tree.root == NULL) {
        return NULL;
    }
    BinTreeNode *node = tree->tree.root;
    while (node->right != NULL) {
        node = node->right;
    }
    return node->data;
}
