#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lfbinarytree.h"

void bintree_init(BinTree *tree, void (*destroy)(void *data)) {
    tree->size = 0;
    tree->destroy = destroy;
    tree->root = NULL;
}

void bintree_destroy(BinTree *tree) {
    bintree_rem_left(tree, NULL);
    memset(tree, 0, sizeof(BinTree));
}

int bintree_ins_left(BinTree *tree, BinTreeNode *node, void *data) {
    BinTreeNode *new_node;
    BinTreeNode **pos;

    if (node == NULL) {
        if (tree->size > 0) {
            return -1;
        }
        pos = &tree->root;
    } else {
        if (node->left != NULL) {
            return -1;
        }
        pos = &node->left;
    }

    if ((new_node = malloc(sizeof(BinTreeNode))) == NULL) {
        return -1;
    }

    if (tree->size == SIZE_MAX) {
        free(new_node);
        return -1;
    }

    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    *pos = new_node;

    tree->size += 1;
    return 0;
}

int bintree_ins_right(BinTree *tree, BinTreeNode *node, void *data) {
    BinTreeNode *new_node;
    BinTreeNode **pos;

    if (node == NULL) {
        if (tree->size > 0) {
            return -1;
        }
        pos = &tree->root;
    } else {
        if (node->right != NULL) {
            return -1;
        }
        pos = &node->right;
    }

    if ((new_node = malloc(sizeof(BinTreeNode))) == NULL) {
        return -1;
    }

    if (tree->size == SIZE_MAX) {
        free(new_node);
        return -1;
    }

    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    *pos = new_node;

    tree->size += 1;
    return 0;
}

void bintree_rem_left(BinTree *tree, BinTreeNode *node) {
    if (tree->size == 0) {
        return;
    }

    BinTreeNode **pos;
    if (node == NULL) {
        pos = &tree->root;
    } else {
        pos = &node->left;
    }

    if (*pos != NULL) {
        bintree_rem_left(tree, *pos);
        bintree_rem_right(tree, *pos);
        if (tree->destroy != NULL) {
            tree->destroy((*pos)->data);
        }
        free(*pos);
        *pos = NULL;
        tree->size -= 1;
    }
}

void bintree_rem_right(BinTree *tree, BinTreeNode *node) {
    if (tree->size == 0) {
        return;
    }

    BinTreeNode **pos;
    if (node == NULL) {
        pos = &tree->root;
    } else {
        pos = &node->right;
    }

    if (*pos != NULL) {
        bintree_rem_left(tree, *pos);
        bintree_rem_right(tree, *pos);
        if (tree->destroy != NULL) {
            tree->destroy((*pos)->data);
        }
        free(*pos);
        *pos = NULL;
        tree->size -= 1;
    }
}

int bintree_merge(BinTree *merge, BinTree *left, BinTree *right, void *data) {
    if (merge == NULL || left == NULL || right == NULL) {
        return -1;
    }
    if (merge == left || merge == right || left == right) {
        return -1;
    }
    if (merge->root != NULL || merge->size != 0) {
        return -1;
    }
    if (left->destroy != right->destroy) {
        return -1;
    }
    if (left->size > SIZE_MAX - 1 || right->size > SIZE_MAX - 1 - left->size) {
        return -1;
    }

    merge->destroy = left->destroy;
    if (bintree_ins_left(merge, NULL, data) != 0) {
        return -1;
    }

    merge->root->left = left->root;
    merge->root->right = right->root;
    merge->size = 1 + left->size + right->size;

    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;
    return 0;
}

void bintree_traverse(BinTree *tree, BinTreeNode *node, int order,
                      void (*visitor)(void *data)) {
    (void)tree;
    if (node == NULL) {
        return;
    }

    switch (order) {
    case BINTREE_PREORDER:
        visitor(node->data);
        bintree_traverse(tree, node->left, order, visitor);
        bintree_traverse(tree, node->right, order, visitor);
        break;
    case BINTREE_INORDER:
        bintree_traverse(tree, node->left, order, visitor);
        visitor(node->data);
        bintree_traverse(tree, node->right, order, visitor);
        break;
    case BINTREE_POSTORDER:
        bintree_traverse(tree, node->left, order, visitor);
        bintree_traverse(tree, node->right, order, visitor);
        visitor(node->data);
        break;
    }
}

static void print_node(char *prefix, BinTreeNode *node, int is_left,
                        void (*print_fn)(void *data, char *buf, size_t buf_sz)) {
    if (node != NULL) {
        char label[64];
        print_fn(node->data, label, sizeof(label));
        printf("%s%s%s\n", prefix, (is_left ? "├──" : "└──"), label);

        char new_prefix[256];
        snprintf(new_prefix, sizeof(new_prefix), "%s%s",
                 prefix, (is_left ? "│   " : "    "));
        print_node(new_prefix, node->left, 1, print_fn);
        print_node(new_prefix, node->right, 0, print_fn);
    }
}

static void default_print_int(void *data, char *buf, size_t buf_sz) {
    snprintf(buf, buf_sz, "%d", *(int *)data);
}

void bintree_debug_print(BinTree *tree,
                         void (*print_fn)(void *data, char *buf, size_t buf_sz)) {
    if (print_fn == NULL) {
        print_fn = default_print_int;
    }
    print_node("", tree->root, 0, print_fn);
}
