Integer helpers, a 2D `LfPoint`, and Bresenham line generation.

## Usage

Create points by value with `lf_point`. Use `bresenham_p` to generate a line between two points, including both endpoints. The caller frees the returned array.

```c
LfPoint start = lf_point(0, 0);
LfPoint end = lf_point(3, 2);
size_t count;
LfPoint *line = bresenham_p(start, end, &count);
if (line != NULL) {
    for (size_t i = 0; i < count; i++) {
        printf("%d,%d\n", line[i].x, line[i].y);
    }
    free(line);
}
```

## Structs

### LfPoint

A point with integer coordinates.

```c
typedef struct {
    int x;
    int y;
} LfPoint;
```

`lf_point` constructs by value. `LfPoint_eq` and `LfPoint_eq_p` return `1` if equal. `LfPoint_eq_p` treats two NULL pointers as equal and one NULL pointer as unequal. They are not `qsort` comparators. `LfPoint_match` returns `0` if equal and is the callback to pass to `List` or `Set`.

```c
LfPoint lf_point(int x, int y);
int LfPoint_eq(LfPoint a, LfPoint b);
int LfPoint_eq_p(const LfPoint *a, const LfPoint *b);
int LfPoint_match(const void *a, const void *b);
```

## Functions

### max_int

Returns the greater of `a` and `b`.

```c
int max_int(int a, int b);
```

### min_int

Returns the lesser of `a` and `b`.

```c
int min_int(int a, int b);
```

### clamp_int

Clamps an integer between a high and low value. If `low > high`, they are swapped.

```c
int clamp_int(int i, int low, int high);
```

### binstr_to_int

Converts a string representing a binary number into an integer. Supports underscores as visual separators. Returns `-1` if `s` is `NULL`, if there are no `0`/`1` digits (empty or only underscores), if any character is not `'0'`, `'1'`, or `'_'`, or if the value does not fit in `int`. `-1` is only an error code; overflow is rejected so a valid result is never negative.

```c
int binstr_to_int(const char *s);

/* Usage */
int a = binstr_to_int("10011101");       // 157
int b = binstr_to_int("1001_1101_0010_1011");  // 40235
binstr_to_int(NULL);    // -1 (null)
```

### bresenham

Uses Bresenham's line algorithm to generate an array of `LfPoint`, from the first endpoint to the second, including both. Identical endpoints produce one point. The caller frees the array. `sz` receives the number of points and is set to zero on failure. Returns `NULL` if `sz` is NULL or if the array cannot be allocated.

```c
LfPoint *bresenham(int x0, int y0, int x1, int y1, size_t *sz);
```

### bresenham_p

Works the same as `bresenham()` but uses `LfPoint` instead of `int`.

```c
LfPoint *bresenham_p(LfPoint p1, LfPoint p2, size_t *sz);
```

### abs_int

Returns the absolute value of an integer. Clamps `INT_MIN` to `INT_MAX` (since `INT_MIN` has no positive representation in two's complement).

```c
int abs_int(int a);
```

### lf_gcd

Returns the greatest common divisor of `a` and `b` using the Euclidean algorithm. Handles negative inputs, including `INT_MIN`. Returns `-1` if the result does not fit in `int`.

```c
int lf_gcd(int a, int b);

/* Usage */
lf_gcd(12, 8);  // returns 4
lf_gcd(7, 13);  // returns 1
```

### lf_lcm

Returns the least common multiple of `a` and `b`. Returns `0` if either input is `0`. Handles negative inputs. Returns `-1` if the result does not fit in `int` (including nonzero inputs involving `INT_MIN`).

```c
int lf_lcm(int a, int b);

/* Usage */
lf_lcm(4, 6);  // returns 12
```

### lf_lerp

Linear interpolation between `a` and `b` by factor `t`. Returns `a` when `t == 0`, `b` when `t == 1`.

```c
float lf_lerp(float a, float b, float t);

/* Usage */
lf_lerp(0.0f, 10.0f, 0.5f);  // returns 5.0
```

### lf_isqrt

Returns the integer square root (floor) of `n`. Returns `-1` for negative inputs. Safe for `INT_MAX`.

```c
int lf_isqrt(int n);

/* Usage */
lf_isqrt(25);  // returns 5
lf_isqrt(26);  // returns 5
```
