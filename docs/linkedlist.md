# linkedlist

A dual sided linked list structure. Used as the foundation for many of the structures in `libflint`

## Usage

Create the list. The user is responsible for memory management of the `List` struct.

```c
List *list = malloc(sizeof(List));
```

After the tree is created, init it. The second argument on `bintree_init()` is an optional memory freeing function pointer
with signature `void (*destroy)(void *data)`. Use `free()` from the stdlib if you are creating the data with `malloc()`.
If allocation of your data is more complex, you can pass your own memory deallocation function as long as it fits the
signature.

In this example, we are passing `NULL` because all memory will be stack allocated.

```c
ll_init(list, NULL);
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
/* node is the 2nd node in the list with value 2 */
ListNode *node = list->head;
/* void pointer to the data inside of the node */
void *data;
ll_remove(list, node, &data);
/* List is now 1 4 */

printf("Removed: %d\n", *((int *) data));
/* Prints "2" */
```

Here is an alternative example using `ll_remove_next()` instead of `ll_remove()`
```c
/* node is the 2nd node in the list with value 2 */
ListNode *node = list->head;
void *data; /* void pointer to the data inside of the node */
ll_remove_next(list, node, &data);
/* List is now 1 2 */

printf("Removed: %d\n", *((int *) data));
/* Prints "4" */
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
ll_init(list, NULL);

int i = 1;
int j = 2;
int k = 4;

ll_ins_next(list, list->head, (void *) &i);
ll_ins_next(list, list->tail, (void *) &j);
ll_ins_next(list, list->tail, (void *) &k);

printf("List: ");
print_ll(list);

void *data;
ll_remove_next(list, list->head, &data);

printf("List: ");
print_ll(list);
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

- `size`: How many nodes are in the list
- `destroy`: Optional deallocation function for member data. Use `NULL` if data is stack allocated
- `match`: Comparison function between data inside two nodes. This is not used by `List` itself, but is used in structures built on top of `List`
- `head`: Pointer to the `ListNode` at the head of the list
- `tail`: Pointer to the `ListNode` at the tail of the list

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
- `next`: Pointer to the `ListNode` before this node in the list
- `prev`: Pointer to the `ListNode` after this node in the list

## Functions

### ll_init

Initalize the list. User is responsible for freeing memory with `ll_destroy()`.

```c
void ll_init(List *list, void (*destroy)(void *data));
```

### ll_destroy

Destroys the nodes inside a list and calls the deallocation funciton on the data if one was provided. Does not destroy
the list itself, that is left up to the user.

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

### ll_remove

Removes and deallocates the node pointed to by `node`. The user is responsible for managing the contained data's memory,
as such the `destroy` function is **not** run by `ll_remove()`

```c
int ll_remove(List *list, ListNode *node, void **data);
```

### ll_remove_next

Removes and deallocates the node pointed to by `node`'s `tail` pointer. The user is responsible for managing the contained data's memory,
as such the `destroy` function is **not** run by `ll_remove_next()`

```c
int ll_remove_next(List *list, ListNode *node, void **data);
```

### ll_remove_prev

Removes and deallocates the node pointed to by `node`'s `head` pointer. The user is responsible for managing the contained data's memory,
as such the `destroy` function is **not** run by `ll_remove_prev()`

```c
int ll_remove_prev(List *list, ListNode *node, void **data);
```

## Macros

### LL_ITER

A utility macro that helps with iterating over an entire list. A `ListNode` pointer named `node` is created as part of
this macro and can be used to access the current node in the iteration, so be careful your own variable naming if you
intend to use `LL_ITER`.

```c
#define LL_ITER(list) for(ListNode *node = (list)->head; node != NULL; node = node->next)

/* Example with list of strings */
for LL_ITER(list) {
    printf("%s\n", (char *)(node->data));
}
```