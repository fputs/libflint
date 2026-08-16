Searchable binary search tree layered on the [[binarytree]] node kit. Insert walks with a qsort-style `cmp` and calls `bintree_ins_left` / `bintree_ins_right`. Remove splices one node; it does not prune a subtree.

This is the `insert(key)` layer. The kit underneath is still a forest constructor, not a BST.

## Structs

```c
typedef struct {
    BinTree tree;
    int (*cmp)(const void *a, const void *b);
} Bst;
```

`cmp` is required. It returns `<0` if `a < b`, `0` if equal, `>0` if `a > b`.

## Functions

### bst_init

Initializes an empty tree. Returns `0` on success, `-1` if `tree` or `cmp` is NULL.

```c
int bst_init(Bst *tree, int (*cmp)(const void *a, const void *b),
             void (*destroy)(void *data));
```

### bst_destroy

Destroys the nodes via `bintree_destroy`. Payloads are destroyed only if a `destroy` callback was given. Does not free the `Bst` handle.

```c
void bst_destroy(Bst *tree);
```

### bst_insert

Inserts `data`. Returns `0` if inserted, `1` if `cmp` says it is already present, `-1` on error.

```c
int bst_insert(Bst *tree, void *data);
```

### bst_find

Returns the stored payload equal to `key`, or NULL if missing.

```c
void *bst_find(const Bst *tree, const void *key);
```

### bst_remove

Removes the node matching `*data` and writes the stored payload back through `data`. Does not call `destroy` — ownership transfers to the caller. Returns `0` on success, `-1` if missing.

```c
int bst_remove(Bst *tree, void **data);
```

### bst_min / bst_max

Return the leftmost / rightmost payload, or NULL if the tree is empty.

```c
void *bst_min(const Bst *tree);
void *bst_max(const Bst *tree);
```

## Macros

```c
#define bst_size(t) ((t)->tree.size)
#define bst_root(t) ((t)->tree.root)
```

In-order walk uses the kit: `bintree_traverse(&t.tree, bst_root(&t), BINTREE_INORDER, visitor)`.
