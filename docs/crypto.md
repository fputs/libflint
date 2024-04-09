# crypto

Cryptographic tools

## Functions

### b64_encode

Encodes an `unsigned char *` into base64. User is responsible for freeing the 
`char *` that is returned.

```c
char *b64_encode(const unsigned char *s, size_t sz);
```

### b64_decode

Decodes a base64 encoded set of bytes into an `unsigned char *`. User is responsible
for freeing the `unsigned char *` that is returned. `decode_sz` is an optional argument
in case you need to know the size of the decoded data, otherwise you can just pass `NULL`.

```c
unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz);
```

### hex_encode

Encodes an array of bytes into a string in hex representation. For example, converts 
`[0xDE, 0xAD, 0xBE, 0xEF]` into `"DEADBEEF"`. The returned string will always be in capital
letters with no leading `0x`. User is responsible for the freeing of the returned string

```c
const char *hex_encode(const unsigned char *hex, size_t sz);

// Example
const char *s = hex_encode(h, 4);
assert(strcmp(s, "DEADBEEF") == 0);
free(s);
```

### hex_decode

Decodes a string of characters representing hexadecimal bytes into an array of `unsigned char`.
For example, converts `"DEADBEEF"` into `[0xDE, 0xAD, 0xBE, 0xEF]`. User is reponsible for the
freeing of the returned byte array

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
const char *hex_to_str(const unsigned char *hex, size_t sz);
```

###  repeating_key_xor

Performs a repeating-key XOR encryption on an array of bytes. User is responsible for freeing the returned array 

```c
const unsigned char* repeating_key_xor(const unsigned char* s, size_t s_sz, const unsigned char* key, size_t k_sz);
```

### repeating_key_xor_s

Performs a repeating-key XOR encryption on a string. Returns the encrypted string as an array of bytes. User is 
responsible for freeing the returned array

```c
const unsigned char *repeating_key_xor_s(const char* s, const char* key);
```
