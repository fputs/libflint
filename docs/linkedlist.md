A dual sided linked list structure. Used as the foundation for many other modules in `libflint`
## Usage

Create the list. The user is responsible for memory management of the `List` struct.

```c
List *list = malloc(sizeof(List));
```

After the list is created, init it. The second argument on `ll_init()` is an optional memory freeing function pointer
with signature `void (*destroy)(void *data)`. Use `free()` from the stdlib if you are creating the data with `malloc()`.
If allocation of your data is more complex, you can pass your own memory deallocation function as long as it fits the
signature. The third argument is an optional `match` callback used by `ll_find()` — pass `NULL` if not needed.

In this example, we are passing `NULL` for both because all memory will be stack allocated and we don't need find.

```c
ll_init(list, NULL, NULL);
int i = 1;
int j = 2;
int k = 4;
```

Next, insert data into the list. Data can be inserted from either side of a node.

```c
ll_ins_next(list, list->head, (void *) &i);
ll_ins_next(list, list->tail, (void *) &j);
ll_ins_next(list, list->tail, (void *) &k);
/* List is now 1 2 4 */
```

Lists can have a node removed by either targeting the node directly or removing a node pointed to by the head or tail of
another node. The user is responsible for the memory management of the data inside the node, which is why a `void*` must
be supplied to hold the data. Since in this example our memory is stack allocated, we don't need to worry about freeing
the `void*` but one must still be supplied for the function to work.

```c
/* List is 1 2 4 */
ListNode *node = list->head->next; /* node with value 2 */
void *data;
ll_remove(list, node, &data);
/* List is now 1 4 */

printf("Removed: %d\n", *((int *) data));
/* Prints "2" */
```

Here is an alternative example using `ll_remove_next()` instead of `ll_remove()`
```c
/* List is 1 2 4 */
ListNode *node = list->head; /* node with value 1 */
void *data;
ll_remove_next(list, node, &data);
/* List is now 1 4 */

printf("Removed: %d\n", *((int *) data));
/* Prints "2" */
```

To destroy the list, first call `ll_destroy()` to free the nodes in the list and optionally run the destroy function
against the data stored in the list. Since this example is stack allocated and we passed `NULL` when creating the
list, no destroy function is run against the memory in the nodes. The list itself must be deleted with `free()`

```c
ll_destroy(list);
free(list);
```

Complete example:

```c
List *list = malloc(sizeof(List));
ll_init(list, NULL, NULL);

int i = 1;
int j = 2;
int k = 4;

ll_ins_next(list, list->head, (void *) &i);
ll_ins_next(list, list->tail, (void *) &j);
ll_ins_next(list, list->tail, (void *) &k);

void *data;
ll_remove_next(list, list->head, &data);
printf("Removed: %d\n", *((int *) data));

ll_destroy(list);
free(list);
```

## Structs

### List

Double ended linked list struct

```c
typedef struct {
    size_t size;
    void (*destroy)(void *data);
    int (*match)(const void *a, const void *b);
    struct ListNode *head;
    struct ListNode *tail;
} List;
```

Members:

- `size`: How many nodes are in the list (read-only after `ll_init`)
- `destroy`: Optional deallocation function for member data. Use `NULL` if data is stack allocated
- `match`: Comparison function between data inside two nodes. Used by `ll_find()` and by structures built on top of `List` such as `Set`
- `head`: Pointer to the `ListNode` at the head of the list (read-only after `ll_init`)
- `tail`: Pointer to the `ListNode` at the tail of the list (read-only after `ll_init`)

`size`, `head`, and `tail` are maintained by the insert/remove helpers. Do not write them directly. The struct stays public so pool-style and cursor insert can read the fields.

### ListNode

Node of the list

```c
typedef struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;
```

Members:

- `data`: void pointer to the member data in the node
- `next`: Pointer to the `ListNode` after this node in the list
- `prev`: Pointer to the `ListNode` before this node in the list

## Functions

### ll_init

Initialize the list. User is responsible for creating the initial `List` structure and freeing memory with `ll_destroy()`. `destroy` is a deallocation function for the members of `List`, pass `NULL` if the memory is stack-allocated. `match` is an optional comparison function used by `ll_find()` — it should return `0` when two elements are equal (comparator convention). Pass `NULL` if not needed; `ll_find` will fall back to pointer comparison.

```c
void ll_init(List *list, void (*destroy)(void *data),
             int (*match)(const void *a, const void *b));

/* Usage */
List *list = malloc(sizeof(List));

// Pass NULL for stack-allocated memory, no match function
ll_init(list, NULL, NULL);

// Pass free and a match function for heap allocated memory
ll_init(list, free, my_match_func);
```

### ll_destroy

Destroys the nodes inside a list and calls the deallocation function on the data if one was provided. Does not destroy the list itself, that is left up to the user.

```c
void ll_destroy(List *list);
```

### ll_ins_next

Creates a new node containing `data` and inserts it after `node`.

```c
int ll_ins_next(List *list, ListNode *node, const void *data);
```

### ll_ins_prev

Creates a new node containing `data` and inserts it before `node`.

```c
int ll_ins_prev(List *list, ListNode *node, const void *data);
```

### ll_push_head / ll_push_tail

Insert at the ends without naming a cursor. Empty lists are allowed. Returns `0` on success, `-1` on error. Stack and queue are implemented only in terms of these and the matching pops.

```c
int ll_push_head(List *list, const void *data);
int ll_push_tail(List *list, const void *data);

/* Usage */
ll_push_tail(list, &i);
ll_push_tail(list, &j);
```

### ll_pop_head / ll_pop_tail

Remove the end node and store its payload in `data`. Returns `0` on success, `-1` if the list is empty or `data` is `NULL`. The destroy callback is not run.

```c
int ll_pop_head(List *list, void **data);
int ll_pop_tail(List *list, void **data);
```

### ll_remove

Removes and deallocates the node pointed to by `node`. Returns `-1` if `node` or `data` is `NULL` or the list is empty. The user is responsible for managing the contained data's memory,
as such the `destroy` function is **not** run by `ll_remove()`

```c
int ll_remove(List *list, ListNode *node, void **data);
```

### ll_remove_next

Removes and deallocates the node after `node` (via its `next` pointer). Returns `-1` if `node` is `NULL` or has no next node. The user is responsible for managing the contained data's memory,
as such the `destroy` function is **not** run by `ll_remove_next()`

```c
int ll_remove_next(List *list, ListNode *node, void **data);
```

### ll_remove_prev

Removes and deallocates the node before `node` (via its `prev` pointer). Returns `-1` if `node` is `NULL` or has no previous node. The user is responsible for managing the contained data's memory,
as such the `destroy` function is **not** run by `ll_remove_prev()`

```c
int ll_remove_prev(List *list, ListNode *node, void **data);
```

### ll_find

Searches the list for the first node whose data matches `data` using the `match` callback on the `List` struct. The match function should return `0` when two elements are equal (comparator convention). If `match` is `NULL`, falls back to pointer comparison. Returns a pointer to the matching `ListNode`, or `NULL` if no match is found.

```c
ListNode *ll_find(const List *list, const void *data);

/* Usage */
int match_int(const void *a, const void *b) {
    return *(const int *)a == *(const int *)b ? 0 : 1;
}

ll_init(list, NULL, match_int);
int key = 2;
ListNode *node = ll_find(list, &key);
```

### ll_reverse

Reverses the list in-place by swapping all `prev`/`next` pointers. O(n) time, O(1) space.

```c
void ll_reverse(List *list);
```

### ll_sort

Sorts the list in-place using merge sort. Requires a comparator function that returns a negative value if `a < b`, `0` if `a == b`, or a positive value if `a > b`. Stable sort, O(n log n) time. All `prev`/`next` pointers and `head`/`tail` are correctly maintained after sorting.

```c
void ll_sort(List *list, int (*cmp)(const void *a, const void *b));

/* Usage */
int cmp_int(const void *a, const void *b) {
    int x = *(int *)a, y = *(int *)b;
    return (x > y) - (x < y);
}

ll_sort(list, cmp_int);
```

### ll_to_array

Allocates and returns an array of `void *` pointers to the data in each node, in list order. The allocation is overflow-checked. The caller is responsible for freeing the returned array. Returns `NULL` if the list is empty or on allocation failure.

```c
void **ll_to_array(const List *list);

/* Usage */
void **arr = ll_to_array(list);
for (size_t i = 0; i < list->size; i++) {
    printf("%d\n", *(int *)arr[i]);
}
free(arr);
```

## Macros

### LL_FOREACH

Iterates the list with a caller-named cursor so loops can nest.

```c
#define LL_FOREACH(list, n) \
    for (ListNode *n = (list)->head; (n) != NULL; (n) = (n)->next)

/* Example with list of strings */
LL_FOREACH(list, n) {
    printf("%s\n", (char *)(n->data));
}
```

### LL_FOREACH_REV

Same as `LL_FOREACH` but walks from tail to head.

```c
#define LL_FOREACH_REV(list, n) \
    for (ListNode *n = (list)->tail; (n) != NULL; (n) = (n)->prev)
```

### LL_ITER

Compatibility alias for `LL_FOREACH(list, node)`. The cursor is always named `node`. Prefer `LL_FOREACH`.

```c
#define LL_ITER(list) LL_FOREACH(list, node)

/* Example */
LL_ITER(list) {
    printf("%s\n", (char *)(node->data));
}
```

### LL_ITER_REV

Compatibility alias for `LL_FOREACH_REV(list, node)`.

```c
#define LL_ITER_REV(list) LL_FOREACH_REV(list, node)
```