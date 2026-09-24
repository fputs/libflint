Bit manipulation macros for setting, clearing, toggling, testing, extracting, and inserting bit fields. Header-only module.

Masks use `uint64_t`, so `LF_BIT_EXTRACT(x, 0, 32)` and `width == 64` are defined. Bit index `i` must be in `0..63`. Some arguments to `LF_BIT_INSERT` are evaluated more than once. Do not pass `arr[i++]`.

## Usage

Include `lfbitops.h` and use an unsigned integer for the bits. Set and clear macros modify that integer in place.

```c
uint64_t flags = 0;
LF_BIT_SET(flags, 3);
LF_BIT_INSERT(flags, 8, 4, 0xA);
uint64_t field = LF_BIT_EXTRACT(flags, 8, 4);  /* field == 0xA */
```

Keep `start` in `0..63` and choose a field that fits the destination: `width <= 64 - start`. Use `start == 0` for a 64-bit field. Bits above the width of a smaller destination are discarded on assignment.

## Functions

### LF_BIT_SET

Sets bit `i` in `x`. Modifies `x` in place.

```c
#define LF_BIT_SET(x, i)

/* Usage */
uint32_t flags = 0;
LF_BIT_SET(flags, 3);  // flags == 0x08
```

### LF_BIT_CLEAR

Clears bit `i` in `x`. Modifies `x` in place.

```c
#define LF_BIT_CLEAR(x, i)

/* Usage */
uint32_t flags = 0xFF;
LF_BIT_CLEAR(flags, 0);  // flags == 0xFE
```

### LF_BIT_TOGGLE

Toggles bit `i` in `x`. Modifies `x` in place.

```c
#define LF_BIT_TOGGLE(x, i)

/* Usage */
uint32_t flags = 0;
LF_BIT_TOGGLE(flags, 4);  // flags == 0x10
LF_BIT_TOGGLE(flags, 4);  // flags == 0x00
```

### LF_BIT_TEST

Tests whether bit `i` is set in `x`. Returns non-zero if set, zero if not. Does not modify `x`.

```c
#define LF_BIT_TEST(x, i)

/* Usage */
uint32_t flags = 0xA5;  // 10100101
LF_BIT_TEST(flags, 0);  // non-zero (set)
LF_BIT_TEST(flags, 1);  // 0 (not set)
```

### LF_BIT_EXTRACT

Extracts a `width`-bit field starting at bit `start` from `x`. Returns the extracted value shifted down to bit 0.

```c
#define LF_BIT_EXTRACT(x, start, width)

/* Usage */
uint32_t reg = 0xDEADBEEF;
LF_BIT_EXTRACT(reg, 0, 4);    // 0xF  (low nibble)
LF_BIT_EXTRACT(reg, 4, 4);    // 0xE  (second nibble)
LF_BIT_EXTRACT(reg, 8, 8);    // 0xBE (second byte)
LF_BIT_EXTRACT(reg, 16, 16);  // 0xDEAD (upper half)
```

### LF_BIT_INSERT

Inserts `val` into a `width`-bit field starting at bit `start` in `x`. Modifies `x` in place. Bits outside the field are not affected. If `val` exceeds the field width, it is clipped to fit.

```c
#define LF_BIT_INSERT(x, start, width, val)

/* Usage */
uint32_t reg = 0;
LF_BIT_INSERT(reg, 0, 4, 0xA);   // reg == 0x0A
LF_BIT_INSERT(reg, 4, 4, 0xB);   // reg == 0xBA

/* Oversized value is clipped */
reg = 0;
LF_BIT_INSERT(reg, 0, 3, 0xFF);  // reg == 0x07 (only low 3 bits)
```

### lf_bit_mask

Returns a mask with the lowest `width` bits set. Zero returns zero; widths of 64 or more return all 64 bits set.

```c
static inline uint64_t lf_bit_mask(unsigned width);
```

### lf_is_pow2

Returns `1` if `x` is a power of two, otherwise `0`. Zero is not a power of two.

```c
static inline int lf_is_pow2(uint64_t x);
```
