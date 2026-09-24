Repeating-key XOR, Hamming distance, and a simple English-likelihood scorer. These are encoding and puzzle helpers, not cryptographic primitives.

## Usage

Pass byte buffers and their lengths to `repeating_key_xor`. The result has the same length as the input and is not NUL-terminated. Free it after use.

```c
unsigned char data[] = {0x01, 0x02, 0x03};
unsigned char key[] = {0xFF};
unsigned char *out = repeating_key_xor(data, sizeof data, key, sizeof key);
if (out != NULL) {
    /* out contains {0xFE, 0xFD, 0xFC} */
    free(out);
}
```

## Functions

### repeating_key_xor

Performs a repeating-key XOR on an array of bytes. Returns `NULL` if the key size is zero, the key is NULL, the source is NULL with a nonzero size, or allocation fails. Empty input returns a freeable buffer. The caller frees the returned array.

```c
unsigned char *repeating_key_xor(const unsigned char *s, size_t s_sz,
                                 const unsigned char *key, size_t k_sz);
```

### repeating_key_xor_s

Uses the lengths of two NUL-terminated strings for repeating-key XOR. Returns `NULL` for NULL arguments or an empty key. The output contains `strlen(s)` raw bytes, without a NUL terminator; the caller frees it.

```c
unsigned char *repeating_key_xor_s(const char *s, const char *key);
```

### hamming_distance

Number of differing bits between two byte arrays of length `sz`. Returns `-1` for invalid inputs or if the result exceeds `INT_MAX`.

```c
int hamming_distance(const unsigned char *a, const unsigned char *b, size_t sz);
```

### hamming_distance_s

Hamming distance between two strings. Returns `-1` if either argument is NULL, the lengths differ, or the distance exceeds `INT_MAX`.

```c
int hamming_distance_s(const char *a, const char *b);
```

### simple_english_scoring

Scores a NUL-terminated string using case-insensitive weights for the letters in ETAOIN SHRDLU and space. Other characters add zero. Higher scores suggest English text, but the total is not normalized for length. Returns `-1` if input is `NULL` or the score exceeds `INT_MAX`.

```c
int simple_english_scoring(const char *s);
```
