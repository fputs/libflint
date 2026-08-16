Cryptopals-style byte puzzles: repeating-key XOR, Hamming distance, and a toy English-likelihood scorer. This is not a cryptography library. Do not grow it into AES, SHA, or an RNG.

## Functions

### repeating_key_xor

Performs a repeating-key XOR on an array of bytes. Returns `NULL` if the key size is `0`. The caller frees the returned array.

```c
unsigned char *repeating_key_xor(const unsigned char *s, size_t s_sz,
                                 const unsigned char *key, size_t k_sz);

/* Usage */
unsigned char data[] = {0x01, 0x02, 0x03};
unsigned char key[] = {0xFF};
unsigned char *out = repeating_key_xor(data, 3, key, 1);
/* out: {0xFE, 0xFD, 0xFC} */
free(out);
```

### repeating_key_xor_s

Same operation on C strings.

```c
unsigned char *repeating_key_xor_s(const char *s, const char *key);
```

### hamming_distance

Number of differing bits between two byte arrays of length `sz`.

```c
int hamming_distance(const unsigned char *a, const unsigned char *b, size_t sz);
```

### hamming_distance_s

Hamming distance between two strings. Returns `-1` if the lengths differ.

```c
int hamming_distance_s(const char *a, const char *b);
```

### simple_english_scoring

Scores a string on a simple ETAOIN SHRDLU scale. A cheap way to guess whether a buffer looks like English (used with XOR ciphertext). Returns `-1` if input is `NULL`.

```c
int simple_english_scoring(const char *s);
```
