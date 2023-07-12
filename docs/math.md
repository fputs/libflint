# math

General math functions

## Functions

## max_int

Return the maximum integer between `int a` and `int b`

```c
int max_int(int a, int b);
```

## min_int

Return the minimum integer between `int a` and `int b`

```c
int min_int(int a, int b);
```

## clamp_int

Clamps an integer between a high and low value

```c
int clamp_int(int i, int low, int high);
```

## binstr_to_int

Converts a string representing a binary number into an integer. Supports underscores as spacing 

```c
int binstr_to_int(const char *s);

/* Usage */
int a = binstr_to_int("10011101");
int b = binstr_to_int("1001_1101_0010_1011");
```

## bresenham

Uses bresenham's line algorithim to generate a line in 2D space.
Returns a pointer to an array of `Point`.
The `sz` parameter holds the size of the array.

```c
Point *bresenham(int x0, int y0, int x1, int y1, size_t *sz);
```

## bresenham_p

Works the same as `bresenham()` but uses the `Point` struct instead of `int`

```c
Point *bresenham_p(Point p1, Point p2, size_t *sz);
```
