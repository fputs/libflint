Cryptographic tools

Any functions that return string representations of bytes in hexadecimal format will
always be formatted with no leading `0x` and lower case letters
## Functions

### b64_encode

Encodes an `unsigned char *` into base64. User is responsible for freeing the
`char *` that is returned.

```c
char *b64_encode(const unsigned char *s, size_t sz);

/* Usage */
char *encoded = b64_encode((unsigned char *)"Hello", 5);
// encoded: "SGVsbG8="
free(encoded);
```

### b64_decode

Decodes a base64 encoded set of bytes into an `unsigned char *`. User is responsible
for freeing the `unsigned char *` that is returned. `decode_sz` is an optional argument
in case you need to know the size of the decoded data, otherwise you can just pass `NULL`.
Invalid characters, truncated input, and incomplete padding fail closed (`NULL`);
a valid prefix of a corrupt string is not returned. The output is NUL-terminated
but may contain interior NULs — use `decode_sz`, not `strlen`.

```c
unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz);

/* Usage */
size_t out_sz;
unsigned char *decoded = b64_decode("SGVsbG8=", 8, &out_sz);
// decoded: "Hello", out_sz: 5
free(decoded);
```

### hex_encode

Encodes an array of bytes into a string in hex representation. For example, converts
`[0xDE, 0xAD, 0xBE, 0xEF]` into `"deadbeef"`. User is responsible for freeing the returned string.
Returns `NULL` if `sz * 2 + 1` would wrap `size_t`.

```c
char *hex_encode(const unsigned char *hex, size_t sz);

// Example
unsigned char h[4] = {
    0xde, 0xad, 0xbe, 0xef
};
char *s = hex_encode(h, 4);
assert(strcmp(s, "deadbeef") == 0);
free(s);
```

### hex_decode

Decodes a string of characters representing hexadecimal bytes into an array of `unsigned char`.
Characters can be in upper or lower case, and can start with a leading `0x` or `0X`.
For example, converts `"DEADBEEF"` into `[0xde, 0xad, 0xbe, 0xef]`. Odd-length input is
padded with a leading nibble of `0`. Returns `NULL` if `sz` is `NULL` or if any
character is not a hex digit. `*sz` is written only on success. User is responsible for the
freeing of the returned byte array.

```c
unsigned char *hex_decode(const char *orig, size_t *sz);

// Example
unsigned char *s = hex_decode("DEADBEEF", &s_sz);
unsigned char h[4] = {
    0xDE, 0xAD, 0xBE, 0xEF
};
for (size_t i = 0; i < 4; ++i) {
    assert(s[i] == h[i]);
}
free(s);
```

### hex_to_str

Converts an array of `unsigned char` into a string based on the ASCII values of each byte. User is
responsible for freeing the returned string.

```c
char *hex_to_str(const unsigned char *hex, size_t sz);

/* Usage */
unsigned char bytes[] = {0x48, 0x69};
char *s = hex_to_str(bytes, 2);
// s: "Hi"
free(s);
```

### repeating_key_xor

Performs a repeating-key XOR encryption on an array of bytes. Returns `NULL` if key size is `0`. User is responsible for freeing the returned array

```c
unsigned char* repeating_key_xor(const unsigned char* s, size_t s_sz, const unsigned char* key, size_t k_sz);

/* Usage */
unsigned char data[] = {0x01, 0x02, 0x03};
unsigned char key[] = {0xFF};
unsigned char *out = repeating_key_xor(data, 3, key, 1);
// out: {0xFE, 0xFD, 0xFC}
free(out);
```

### repeating_key_xor_s

Performs a repeating-key XOR encryption on a string. Returns the encrypted string as an array of bytes. User is
responsible for freeing the returned array

```c
unsigned char *repeating_key_xor_s(const char* s, const char* key);

/* Usage */
unsigned char *out = repeating_key_xor_s("hello", "key");
free(out);
```

### hamming_distance

Calculates the Hamming Distance (the number of differing bits) between two arrays of unsigned bytes of length `sz`.

```c
int hamming_distance(const unsigned char *a, const unsigned char *b, size_t sz);

/* Usage */
unsigned char a[] = {0xFF};
unsigned char b[] = {0x00};
int d = hamming_distance(a, b, 1);  // returns 8
```

### hamming_distance_s

Calculates the Hamming Distance (the number of differing bits) between two strings. Returns `-1` if the strings have different lengths.

```c
int hamming_distance_s(const char *a, const char *b);

/* Usage */
int d = hamming_distance_s("karolin", "kathrin");  // returns 9
```

