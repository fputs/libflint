Binary tree with standard leaf operations
## Usage

Create the tree. The user is responsible for memory management of the `BinTree` struct.

```c
BinTree *tree = malloc(sizeof(BinTree));
```

After the tree is created, init it. The second argument on `bintree_init()` is an optional memory freeing function pointer with signature `void (*destroy)(void *data)`. Use `free()` from the stdlib if you are creating the data with `malloc()`. If allocation of your data is more complex, you can pass your own memory deallocation function as long as it fits the signature.

In this example, we are passing `NULL` because all memory will be stack allocated.

```c
bintree_init(tree, NULL);
int root = 0;
int l1 = 1;
int l2 = 2;
int r1 = 12;
int r2 = 200;
```

Next lets insert our data into the tree. The insert functions signature is `bintree_ins_...(tree, parent, data)`. If you are inserting data at the root of the tree, you may use either `bintree_ins_left()` or `bintree_ins_right()` as long as
`NULL` is passed as the parent argument.

```c
bintree_ins_left(tree, NULL, &root);
bintree_ins_left(tree, tree->root, &l1);
bintree_ins_left(tree, tree->root->left, &l2);
bintree_ins_right(tree, tree->root->left, &r2);
bintree_ins_right(tree, tree->root, &r1);
bintree_ins_right(tree, tree->root->right, &r2);
bintree_ins_left(tree, tree->root->right, &l1);
```

We can use `bintree_debug_print(tree, NULL)` to print a graphical representation of the tree to `stdout`
```plaintext
└──0
    ├──1
    │   ├──2
    │   └──200
    └──12
        ├──1
        └──200
```

To cleanup the tree, first destroy the nodes. If you passed a deallocation function, it will be called on the data member of each node before the node itself is freed. `bintree_destroy()` does not free the tree itself, just the nodes inside of it, hence we must also call `free()` on the tree.

```c
bintree_destroy(tree);
free(tree);
tree = NULL;
```

Here is the entire example:

```c
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

bintree_debug_print(tree, NULL);

bintree_destroy(tree);
free(tree);
tree = NULL;
```

## Structs

### BinTree

Binary tree struct

```c
typedef struct {
    int size;
    void (*destroy)(void *data);
    struct BinTreeNode *root;
} BinTree;
```

Members:

- `size`: How many nodes the tree contains
- `destroy`: Optional deallocation function for data inside a node. Typical usage is `NULL` for stack allocated data and `free()` for data created with `malloc()`
- `root`: The root node of the tree

### BinTreeNode

Node of the tree

```c
typedef struct BinTreeNode {
    void *data;
    struct BinTreeNode *left;
    struct BinTreeNode *right;
} BinTreeNode;
```

Members:
- `data`: void pointer to data the node contains
- `left`: left facing leaf of the node
- `right`: right facing leaf of the node

## Functions

### bintree_init

Initialize the binary tree. User is responsible for freeing memory with `bintree_destroy()`.

```c
void bintree_init(BinTree *tree, void (*destroy)(void *data))
```

### bintree_destroy

Destroys the nodes inside a tree and calls the deallaction function on the data if one was provided. Does not deallocate
the tree itself, that is left to the user.

```c
void bintree_destroy(BinTree *tree)
```

### bintree_ins_left

Creates a new node containing `data` and inserts it as the left child of `node`.

```c
int bintree_ins_left(BinTree *tree, BinTreeNode *node, void *data)
```

### bintree_ins_right

Creates a new node containing `data` and inserts it as the right child of `node`.

```c
int bintree_ins_right(BinTree *tree, BinTreeNode *node, void *data)
```

### bintree_rem_left

Removes and deallocates the left child node of `node`. Calls the deallocation function on the data if one was provided.

```c
void bintree_rem_left(BinTree *tree, BinTreeNode *node)
```

### bintree_rem_right

Removes and deallocates the right child node of `node`. Calls the deallocation function on the data if one was provided.

```c
void bintree_rem_right(BinTree *tree, BinTreeNode *node)
```

### bintree_merge

Merges two trees into a new tree with `data` at the root. `left` becomes the left subtree and `right` becomes the right subtree. Returns `-1` if `left == right` (same pointer). The merged tree takes ownership of both subtrees.

```c
int bintree_merge(BinTree *merge, BinTree *left, BinTree *right, void *data);
```

### bintree_traverse

Traverses the tree starting at `node` in the specified order, calling `visitor` on each node's data.

```c
void bintree_traverse(BinTree *tree, BinTreeNode *node, int order,
                      void (*visitor)(void *data));

/* Traversal orders */
BINTREE_PREORDER   /* root, left, right */
BINTREE_INORDER    /* left, root, right */
BINTREE_POSTORDER  /* left, right, root */

/* Usage */
void print_int(void *data) {
    printf("%d ", *(int *)data);
}
bintree_traverse(tree, tree->root, BINTREE_INORDER, print_int);
```

### bintree_debug_print

Prints a representation of the tree to stdout. Accepts an optional `print_fn` callback to format node data — pass `NULL` to use the default integer printer. Gets very messy with large trees.

```c
void bintree_debug_print(BinTree *tree,
                         void (*print_fn)(void *data, char *buf, size_t buf_sz));

/* Usage */
bintree_debug_print(tree, NULL);  /* uses default int printer */
```

### bintree_is_eob

Utility macro that checks if the node is the End Of Branch.

```c
#define bintree_is_eob(node) ((node) == NULL)
```

### bintree_is_leaf

Utility macro that checks if a node is a leaf (has no children).

```c
#define bintree_is_leaf(node) ((node)->left == NULL && (node)->right == NULL)
```
