Utility code that does not fit anywhere else
## Structs

### Point

Representation of a point on a two dimensional grid

```c
typedef struct Point {
    int x;
    int y;
} Point;
```

## Functions

### Point_new

Creates and returns a `Point` by value.

```c
Point Point_new(int x, int y);

/* Usage */
Point p = Point_new(10, 20);
```

### Point_new_p

Creates and returns a heap-allocated `Point`. User is responsible for freeing the returned pointer (or using `Point_destroy`). Returns `NULL` on allocation failure.

```c
Point *Point_new_p(int x, int y);

/* Usage */
Point *p = Point_new_p(10, 20);
free(p);
```

### Point_eq

Compares two `Point` values. Returns `1` if they are equal, `0` otherwise.

```c
int Point_eq(const Point a, const Point b);
```

### Point_destroy

Frees a heap-allocated `Point`. Safe to call with `NULL`.

```c
void Point_destroy(Point *p);

/* Usage */
Point *p = Point_new_p(10, 20);
Point_destroy(p);
```

### Point_cmp_p

Compares two `Point` pointers. Returns `1` if they are equal, `0` otherwise.

```c
int Point_cmp_p(const Point *a, const Point *b);
```

### Point_cmp_v

Compares two `Point` pointers passed as `void *`. Suitable as a `match` callback for generic data structures (`List`, `Set`, etc.). Returns `1` if they are equal, `0` otherwise.

```c
int Point_cmp_v(const void *a, const void *b);
```