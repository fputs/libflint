General math functions
## Functions

### max_int

Return the maximum integer between `int a` and `int b`

```c
int max_int(int a, int b);
```

### min_int

Return the minimum integer between `int a` and `int b`

```c
int min_int(int a, int b);
```

### clamp_int

Clamps an integer between a high and low value. If `low > high`, they are swapped

```c
int clamp_int(int i, int low, int high);
```

### binstr_to_int

Converts a string representing a binary number into an integer. Supports underscores as visual separators. Returns `-1` if `s` is `NULL` or if any character is not `'0'`, `'1'`, or `'_'`. Returns `0` for an empty string.

```c
int binstr_to_int(const char *s);

/* Usage */
int a = binstr_to_int("10011101");       // 157
int b = binstr_to_int("1001_1101_0010_1011");  // 40235
binstr_to_int(NULL);    // -1 (null)
```

### bresenham

Uses bresenham's line algorithm to generate a line in 2D space. Returns a pointer to an array of `Point`. The `sz` parameter holds the size of the array.

```c
Point *bresenham(int x0, int y0, int x1, int y1, size_t *sz);
```

### bresenham_p

Works the same as `bresenham()` but uses the `Point` struct instead of `int`

```c
Point *bresenham_p(Point p1, Point p2, size_t *sz);
```

### abs_int

Returns the absolute value of an integer. Clamps `INT_MIN` to `INT_MAX` (since `INT_MIN` has no positive representation in two's complement)

```c
int abs_int(int a);
```

### is_power_of_two

Returns `1` if `i` is a positive power of two, otherwise returns `0`. Returns `0` for `i <= 0`.

```c
int is_power_of_two(int i);
```

### gcd

Returns the greatest common divisor of `a` and `b` using the Euclidean algorithm. Handles negative inputs.

```c
int gcd(int a, int b);

/* Usage */
gcd(12, 8);  // returns 4
gcd(7, 13);  // returns 1
```

### lcm

Returns the least common multiple of `a` and `b`. Returns `0` if either input is `0`. Handles negative inputs.

```c
int lcm(int a, int b);

/* Usage */
lcm(4, 6);  // returns 12
```

### lerp

Linear interpolation between `a` and `b` by factor `t`. Returns `a` when `t == 0`, `b` when `t == 1`.

```c
float lerp(float a, float b, float t);

/* Usage */
lerp(0.0f, 10.0f, 0.5f);  // returns 5.0
```

### isqrt

Returns the integer square root (floor) of `n`. Returns `-1` for negative inputs.

```c
int isqrt(int n);

/* Usage */
isqrt(25);  // returns 5
isqrt(26);  // returns 5
```