A binary tree you build by attaching left and right children. Nodes are not ordered by key. For insert, find, and remove by comparison, see [[bst]].

## Usage

Create the tree. The user is responsible for memory management of the `BinTree` struct.

```c
BinTree *tree = malloc(sizeof(BinTree));
```

After the tree is created, init it. The second argument on `bintree_init()` is an optional memory freeing function pointer with signature `void (*destroy)(void *data)`. Use `free()` from the stdlib if you are creating the data with `malloc()`. If allocation of your data is more complex, you can pass your own memory deallocation function as long as it fits the signature.

This example passes `NULL` because the payloads are stack allocated.

```c
bintree_init(tree, NULL);
int root = 0;
int l1 = 1;
int l2 = 2;
int r1 = 12;
int r2 = 200;
```

Insert with `bintree_ins_left(tree, parent, data)` or `bintree_ins_right`. To insert the root, pass `NULL` as the parent.

```c
bintree_ins_left(tree, NULL, &root);
bintree_ins_left(tree, tree->root, &l1);
bintree_ins_left(tree, tree->root->left, &l2);
bintree_ins_right(tree, tree->root->left, &r2);
bintree_ins_right(tree, tree->root, &r1);
bintree_ins_right(tree, tree->root->right, &r2);
bintree_ins_left(tree, tree->root->right, &l1);
```

We can use `bintree_debug_print(tree, NULL)` to print a graphical representation of the tree to `stdout`.

```plaintext
└──0
    ├──1
    │   ├──2
    │   └──200
    └──12
        ├──1
        └──200
```

`bintree_destroy` frees the nodes. If you passed a `destroy` callback, it is called on each payload first. The `BinTree` handle is yours to free.

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

The tree handle.

```c
typedef struct {
    size_t size;
    void (*destroy)(void *data);
    struct BinTreeNode *root;
} BinTree;
```

Members:

- `size`: How many nodes the tree contains
- `destroy`: Optional deallocation function for data inside a node. Typical usage is `NULL` for stack allocated data and `free()` for data created with `malloc()`
- `root`: The root node of the tree

### BinTreeNode

A node in the tree.

```c
typedef struct BinTreeNode {
    void *data;
    struct BinTreeNode *left;
    struct BinTreeNode *right;
} BinTreeNode;
```

Members:

- `data`: Pointer to the data the node contains
- `left`: Left child, or `NULL` if absent
- `right`: Right child, or `NULL` if absent

## Functions

### bintree_init

Initializes an empty tree with an optional payload deallocation callback. Call `bintree_destroy()` when finished; the tree handle remains yours to free.

```c
void bintree_init(BinTree *tree, void (*destroy)(void *data));
```

### bintree_destroy

Frees every node and calls the deallocation callback on each payload if one was provided. Resets the tree to zero without freeing the tree handle.

```c
void bintree_destroy(BinTree *tree);
```

### bintree_ins_left

Creates a new node containing `data` and inserts it as the left child of `node`. Pass `NULL` as `node` to insert the root of an empty tree. Returns `0` on success, or `-1` if the position is occupied, allocation fails, or the node count would overflow. A non-NULL `node` must belong to this tree.

```c
int bintree_ins_left(BinTree *tree, BinTreeNode *node, void *data);
```

### bintree_ins_right

Creates a new node containing `data` and inserts it as the right child of `node`. The root insertion rule, return values, and parent requirement are the same as `bintree_ins_left()`.

```c
int bintree_ins_right(BinTree *tree, BinTreeNode *node, void *data);
```

### bintree_rem_left

Removes the entire subtree rooted at the left child of `node`. Calls the deallocation callback on each payload if one was provided. Pass `NULL` as `node` to remove the whole tree. A non-NULL `node` must belong to this tree.

```c
void bintree_rem_left(BinTree *tree, BinTreeNode *node);
```

### bintree_rem_right

Removes the entire subtree rooted at the right child of `node`. Calls the deallocation callback on each payload if one was provided. Pass `NULL` as `node` to remove the whole tree. A non-NULL `node` must belong to this tree.

```c
void bintree_rem_right(BinTree *tree, BinTreeNode *node);
```

### bintree_merge

Merges two trees into `merge` with `data` at the root. `merge` must already be initialized and empty. `left` and `right` must be distinct from each other and from `merge`, and `left` and `right` must share the same `destroy` callback. Returns `0` on success, or `-1` for NULL handles, broken contracts, node-count overflow, or allocation failure. The merged tree adopts the source callback and takes ownership of both subtrees, leaving `left` and `right` empty.

```c
int bintree_merge(BinTree *merge, BinTree *left, BinTree *right, void *data);
```

### bintree_traverse

Traverses the tree starting at `node` in the specified order, calling `visitor` on each node's data. A NULL node does nothing; `visitor` must be non-NULL when visiting nodes.

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

Prints a representation of the tree to stdout. Pass a `print_fn` to format node data, or `NULL` to print integer payloads. Large trees produce a lot of output.

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

Utility macro that checks if a node is a leaf (has no children). `NULL` is not a leaf.

```c
#define bintree_is_leaf(node) ((node) != NULL && \
    ((const BinTreeNode *)(node))->left == NULL && \
    ((const BinTreeNode *)(node))->right == NULL)
```
