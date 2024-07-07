#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "lfcrypto.h"

#define B64_BUF_SZ (1024 * 64)

static const char b64_table[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
};

typedef struct {
    unsigned char *ptr;
    size_t count;
} b64_buf;

static int new_b64_buf(b64_buf *b) {
    b->ptr = malloc(B64_BUF_SZ);
    if (b->ptr == NULL) {
        return 1;
    }
    b->count = 1;
    return 0;
}

static int resize_b64_buf(b64_buf *b, size_t sz) {
    if (sz > b->count * B64_BUF_SZ) {
        while (sz > b->count * B64_BUF_SZ) {
           b->count++;
        }
        b->ptr = realloc(b->ptr, B64_BUF_SZ * b->count);
        if (b->ptr == NULL) {
            return 1;
        }
    }
    return 0;
}

const char *b64_encode(const unsigned char *s, size_t sz) {
    int i = 0;
    b64_buf encbuf;
    size_t size = 0;
    unsigned char buf[4];
    unsigned char tmp[3];

    if (new_b64_buf(&encbuf) != 0) {
        return NULL;
    }

    while (sz--) {
        tmp[i++] = *(s++);
        if (i == 3) {
            buf[0] = (tmp[0] & 0xfc) >> 2;
            buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
            buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
            buf[3] = tmp[2] & 0x3f;

            if (resize_b64_buf(&encbuf, size + 4) != 0) {
                free(encbuf.ptr);
                return NULL;
            }

            for (i = 0; i < 4; ++i) {
                encbuf.ptr[size++] = b64_table[buf[i]];
            }

            i = 0;
        }
    }

    if (i > 0) {
        for (int j = i; j < 3; ++j) {
            tmp[j] = '\0';
        }

        buf[0] = (tmp[0] & 0xfc) >> 2;
        buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
        buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
        buf[3] = tmp[2] & 0x3f;

        for (int j = 0; (j < i + 1); ++j) {
            if (resize_b64_buf(&encbuf, size + 1) != 0) {
                free(encbuf.ptr);
                return NULL;
            }
            encbuf.ptr[size++] = b64_table[buf[j]];
        }

        while (i++ < 3) {
            if (resize_b64_buf(&encbuf, size + 1) != 0) {
                free(encbuf.ptr);
                return NULL;
            }
            encbuf.ptr[size++] = '=';
        }
    }

    if (resize_b64_buf(&encbuf, size + 1) != 0) {
        free(encbuf.ptr);
        return NULL;
    }
    encbuf.ptr[size] = '\0';

    return (char *)encbuf.ptr;
}

unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz) {
    int i = 0, j = 0, l = 0;
    size_t size = 0;
    b64_buf decbuf;
    unsigned char buf[3];
    unsigned char tmp[4];

    if (new_b64_buf(&decbuf) != 0) {
        return NULL;
    }

    while (sz--) {
        if (s[j] == '=') {
            break;
        }
        // Not base64 characters
        if (!isalnum(s[j]) || s[j] == '+' || s[j] == '/') {
            break;
        }

        tmp[i++] = s[j++];

        if (i == 4) {
            for (i = 0; i < 4; ++i) {
                for (l = 0; l < 64; ++l) {
                    if (tmp[i] == b64_table[l]) {
                        tmp[i] = l;
                        break;
                    }
                }
            }

            buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
            buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
            buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

            if (resize_b64_buf(&decbuf, size + 3) != 0) {
                free(decbuf.ptr);
                return NULL;
            }

            for (i = 0; i < 3; ++i) {
                decbuf.ptr[size++] = buf[i];
            }

            i = 0;
        }
    }

    if (i > 0) {
        for (j = i; j < 4; ++j) {
            tmp[j] = '\0';
        }

        for (j = 0; j < 4; ++j) {
            for (l = 0; l < 64; ++l) {
                if (tmp[j] == b64_table[l]) {
                    tmp[j] = l;
                    break;
                }
            }
        }

        buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
        buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
        buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

        if (resize_b64_buf(&decbuf, size + (i - 1)) != 0) {
            free(decbuf.ptr);
            return NULL;
        }
        for (j = 0; (j < i - 1); ++j) {
            decbuf.ptr[size++] = buf[j];
        }
    }

    if (resize_b64_buf(&decbuf, size + 1) != 0) {
        free(decbuf.ptr);
        return NULL;
    }
    decbuf.ptr[size] = '\0';

    if (decode_sz != NULL) {
        *decode_sz = size;
    }

    return decbuf.ptr;
}

unsigned char *hex_decode(const char *orig, size_t *sz) {
    size_t buf_sz = strlen(orig) + 1;
    const char *sptr = orig;
    if (strncmp(orig, "0x", 2) == 0) {
        buf_sz -= 2;
        sptr += 2;
    }

    if (strlen(orig) % 2 != 0) {
        buf_sz += 1;
    }

    char *buf = malloc(sizeof(char) * buf_sz);
    if (strlen(sptr) % 2 != 0) {
        strcpy(buf+ 1, sptr);
        buf[0] = '0';
    } else {
        strcpy(buf, sptr);
    }
    buf[buf_sz - 1] = '\0';

    *sz = buf_sz / 2;
    unsigned char *hex = malloc(sizeof(unsigned char) * *sz);
    const char *pos = buf;

    for (size_t i = 0; i < *sz; ++i) {
        sscanf(pos, "%2hhx", &hex[i]);
        pos += 2;
    }

    free(buf);
    return hex;
}

const char *hex_encode(const unsigned char *hex, size_t sz) {
    size_t ssz = sz * 2 + 1;
    char *s = malloc(sizeof(char) * ssz);
    char *pos = s;

    for (size_t i = 0; i < sz; ++i) {
        snprintf(pos, 3, "%02x", hex[i]);
        pos += 2;
    }
    s[ssz - 1] = '\0';

    return s;
}

const char *hex_to_str(const unsigned char *hex, size_t sz) {
    char *s = malloc(sizeof(char) * (sz + 1));
    for (size_t i = 0; i < sz; ++i) {
        s[i] = (char)hex[i];
    }
    s[sz] = '\0';
    return s;
}

const unsigned char* repeating_key_xor(const unsigned char* s, size_t s_sz, const unsigned char* key, size_t k_sz) {
    unsigned char* r = malloc(sizeof(unsigned char) * s_sz);
    for (size_t i = 0, j = 0; i < s_sz; ++i) {
        r[i] = s[i] ^ key[j];
        j = (j + 1) % k_sz;
    }
    return r;
}

const unsigned char *repeating_key_xor_s(const char* s, const char* key) {
    return repeating_key_xor((unsigned char*)s, strlen(s), (unsigned char*)key, strlen(key));
}

unsigned int hamming_distance_s(const char *a, const char *b) {
    size_t sz = strlen(a);
    if (sz != strlen(b)) {
        return -1;
    }

    return hamming_distance((unsigned char *)a, (unsigned char *)b, sz);
}

unsigned int hamming_distance(unsigned char *a, unsigned char *b, size_t sz) {
    unsigned int hamming = 0;
    for (size_t i = 0; i < sz; ++i) {
        if (a[i] == b[i]) {
            continue;
        }
        unsigned char c = a[i] ^ b[i];
        unsigned int count = 0;
        for (; c; count++) {
            c &= c - 1;
        }
        hamming += count;
    }

    return hamming;
}
