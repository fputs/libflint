# crypto

Cryptographic library

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
