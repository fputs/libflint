Ordered binary search tree of `void *`. Insert, find, and remove walk the tree with a qsort-style `cmp`. Removing an element splices that one node and leaves the rest of the tree in order.

To attach children by hand instead of by key, use [[binarytree]].

## Usage

Initialize a `Bst` with a comparator and an optional payload destructor. The comparator receives the stored pointers directly. Pass `NULL` for the destructor when the payloads are stack allocated.

```c
int compare_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

Bst tree;
if (bst_init(&tree, compare_int, NULL) == 0) {
    int value = 42;
    bst_insert(&tree, &value);

    int key = 42;
    void *found = bst_find(&tree, &key);
    /* found points to value if insertion succeeded. */
    (void)found;
    bst_destroy(&tree);
}
```

`bst_destroy` frees the nodes and calls the destructor on remaining payloads. The `Bst` handle is yours to free if you allocated it. Keep comparison keys unchanged while stored. The tree is not balanced, so ordered inserts can make searches linear in the number of nodes.

## Structs

### Bst

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

Inserts `data`. Returns `0` if inserted, `1` if `cmp` says it is already present, `-1` on error. Duplicate or failed inserts leave the payload with the caller.

```c
int bst_insert(Bst *tree, void *data);
```

### bst_find

Returns the stored payload equal to `key`, or `NULL` if missing. A stored `NULL` payload also returns `NULL`.

```c
void *bst_find(const Bst *tree, const void *key);
```

### bst_remove

Removes the node matching `*data` and writes the stored payload back through `data`. Does not call `destroy`; the caller owns the payload after a successful remove. Returns `0` on success, `-1` if missing or if the arguments are invalid.

```c
int bst_remove(Bst *tree, void **data);
```

### bst_min / bst_max

Return the leftmost / rightmost payload, or `NULL` if the tree is empty. A stored `NULL` payload also returns `NULL`.

```c
void *bst_min(const Bst *tree);
void *bst_max(const Bst *tree);
```

## Macros

```c
#define bst_size(t) ((t)->tree.size)
#define bst_root(t) ((t)->tree.root)
```

Walk the tree in order with `bintree_traverse(&t.tree, bst_root(&t), BINTREE_INORDER, visitor)`.
