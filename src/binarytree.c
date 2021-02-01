#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "binarytree.h"

void bintree_init(struct BinTree *tree, void (*destroy)(void *data)) {
    tree->size = 0;
    tree->destroy = destroy;
    tree->compare = NULL;
    tree->root = NULL;
}

void bintree_destroy(struct BinTree *tree) {
    bintree_rem_left(tree, NULL);
    memset(tree, 0, sizeof(struct BinTree));
}

int bintree_ins_left(struct BinTree *tree, struct BinTreeNode *node, void *data) {
    struct BinTreeNode *new_node;
    struct BinTreeNode **pos;

    if (node == NULL) {
        if (tree->size > 0) {
            return 1;
        }
        pos = &tree->root;
    } else {
        if (node->left != NULL) {
            return -1;
        }
        pos = &node->left;
    }

    if ((new_node = malloc(sizeof(struct BinTreeNode))) == NULL) {
        return 2;
    }

    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    *pos = new_node;

    tree->size += 1;
    return 0;
}

int bintree_ins_right(struct BinTree *tree, struct BinTreeNode *node, void *data) {
    struct BinTreeNode *new_node;
    struct BinTreeNode **pos;

    if (node == NULL) {
        if (tree->size > 0) {
            return 1;
        }
        pos = &tree->root;
    } else {
        if (node->right != NULL) {
            return -1;
        }
        pos = &node->right;
    }

    if ((new_node = malloc(sizeof(struct BinTreeNode))) == NULL) {
        return 2;
    }

    new_node->data = data;
    new_node->left = NULL;
    new_node->right = NULL;
    *pos = new_node;

    tree->size += 1;
    return 0;
}

void bintree_rem_left(struct BinTree *tree, struct BinTreeNode *node) {
    if (tree->size == 0) {
        return;
    }

    struct BinTreeNode **pos;
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

void bintree_rem_right(struct BinTree *tree, struct BinTreeNode *node) {
    if (tree->size == 0) {
        return;
    }

    struct BinTreeNode **pos;
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

int bintree_merge(struct BinTree *merge, struct BinTree *left, struct BinTree *right, void *data) {
    bintree_init(merge, left->destroy);
    if (bintree_ins_left(merge, NULL, data) != 0) {
        bintree_destroy(merge);
        return -1;
    }

    merge->root->left = left->root;
    merge->root->right = right->root;

    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;

    return 0;
}

void print_node(char* prefix, struct BinTreeNode *node, int is_left, void (*pfunc)(void* data)) {
    if (node != NULL) {
        printf("%s%s", prefix, (is_left ? "├──" : "└──" ));
        pfunc(node->data);
        char new_prefix[64];
        memset(new_prefix, 0, 64);
        strcat(new_prefix, prefix);
        strcat(new_prefix, (is_left == 1 ? "│   " : "    "));
        print_node(new_prefix, node->left, 1, pfunc);
        print_node(new_prefix, node->right, 0, pfunc);
    }
}

void bintree_debug_pfunc_int(void* data) {
    int i = *((int*)data);
    printf("%d\n", i);
}

void bintree_debug_print(struct BinTree *tree) {
    print_node("", tree->root, 0, bintree_debug_pfunc_int);
}
