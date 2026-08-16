#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lfcompat.h"
#include "lfcrypto.h"

#define B64_BUF_SZ 256

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
    size_t need = b->count;
    while (need <= SIZE_MAX / B64_BUF_SZ && sz > need * B64_BUF_SZ) {
        if (need == SIZE_MAX) {
            return 1;
        }
        need++;
    }
    if (need > SIZE_MAX / B64_BUF_SZ || sz > need * B64_BUF_SZ) {
        return 1;
    }
    if (need == b->count) {
        return 0;
    }
    void *tmp = lf_reallocarray(b->ptr, need, B64_BUF_SZ);
    if (tmp == NULL) {
        return 1;
    }
    b->ptr = tmp;
    b->count = need;
    return 0;
}

static int b64_val(unsigned char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    }
    if (c == '+') {
        return 62;
    }
    if (c == '/') {
        return 63;
    }
    return -1;
}

static int b64_decode_quartet(unsigned char tmp[4], unsigned char buf[3]) {
    int v[4];
    for (int i = 0; i < 4; ++i) {
        v[i] = b64_val(tmp[i]);
        if (v[i] < 0) {
            return -1;
        }
        tmp[i] = (unsigned char)v[i];
    }
    buf[0] = (unsigned char)((tmp[0] << 2) + ((tmp[1] & 0x30) >> 4));
    buf[1] = (unsigned char)(((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2));
    buf[2] = (unsigned char)(((tmp[2] & 0x3) << 6) + tmp[3]);
    return 0;
}

char *b64_encode(const unsigned char *s, size_t sz) {
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
    if (s == NULL && sz != 0) {
        return NULL;
    }

    int i = 0;
    size_t size = 0;
    b64_buf decbuf;
    unsigned char buf[3];
    unsigned char tmp[4];

    if (new_b64_buf(&decbuf) != 0) {
        return NULL;
    }

    size_t n = 0;
    while (n < sz) {
        unsigned char c = (unsigned char)s[n];
        if (c == '=') {
            break;
        }
        if (b64_val(c) < 0) {
            free(decbuf.ptr);
            return NULL;
        }

        tmp[i++] = c;
        n++;

        if (i == 4) {
            if (b64_decode_quartet(tmp, buf) != 0) {
                free(decbuf.ptr);
                return NULL;
            }
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

    if (n < sz && (unsigned char)s[n] == '=') {
        if (i != 2 && i != 3) {
            free(decbuf.ptr);
            return NULL;
        }
        size_t pads = 4 - (size_t)i;
        for (size_t p = 0; p < pads; ++p) {
            if (n >= sz || (unsigned char)s[n] != '=') {
                free(decbuf.ptr);
                return NULL;
            }
            n++;
        }
        if (n != sz) {
            free(decbuf.ptr);
            return NULL;
        }
    } else if (i == 1) {
        free(decbuf.ptr);
        return NULL;
    }

    if (i > 0) {
        for (int j = i; j < 4; ++j) {
            tmp[j] = 'A';
        }
        if (b64_decode_quartet(tmp, buf) != 0) {
            free(decbuf.ptr);
            return NULL;
        }
        if (resize_b64_buf(&decbuf, size + (size_t)(i - 1)) != 0) {
            free(decbuf.ptr);
            return NULL;
        }
        for (int j = 0; j < i - 1; ++j) {
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

static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

unsigned char *hex_decode(const char *orig, size_t *sz) {
    if (orig == NULL || sz == NULL) {
        return NULL;
    }

    const char *sptr = orig;
    if (sptr[0] == '0' && (sptr[1] == 'x' || sptr[1] == 'X')) {
        sptr += 2;
    }

    size_t slen = strlen(sptr);
    int pad = (slen % 2 != 0);
    size_t out_sz = (slen + (size_t)pad) / 2;

    unsigned char *hex = malloc(out_sz == 0 ? 1 : out_sz);
    if (hex == NULL) {
        return NULL;
    }

    size_t out_i = 0;
    size_t in_i = 0;
    if (pad) {
        int lo = hex_nibble(sptr[0]);
        if (lo < 0) {
            free(hex);
            return NULL;
        }
        hex[out_i++] = (unsigned char)lo;
        in_i = 1;
    }

    for (; in_i < slen; in_i += 2) {
        int hi = hex_nibble(sptr[in_i]);
        int lo = hex_nibble(sptr[in_i + 1]);
        if (hi < 0 || lo < 0) {
            free(hex);
            return NULL;
        }
        hex[out_i++] = (unsigned char)((hi << 4) | lo);
    }

    *sz = out_sz;
    return hex;
}

char *hex_encode(const unsigned char *hex, size_t sz) {
    if (sz > (SIZE_MAX - 1) / 2) {
        return NULL;
    }
    if (hex == NULL && sz != 0) {
        return NULL;
    }
    size_t ssz = sz * 2 + 1;
    char *s = malloc(ssz);
    if (s == NULL) {
        return NULL;
    }

    char *pos = s;

    for (size_t i = 0; i < sz; ++i) {
        snprintf(pos, 3, "%02x", hex[i]);
        pos += 2;
    }
    s[ssz - 1] = '\0';

    return s;
}

char *hex_to_str(const unsigned char *hex, size_t sz) {
    char *s = malloc(sizeof(char) * (sz + 1));
    if (s == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < sz; ++i) {
        s[i] = (char)hex[i];
    }
    s[sz] = '\0';
    return s;
}

unsigned char* repeating_key_xor(const unsigned char* s, size_t s_sz, const unsigned char* key, size_t k_sz) {
    if (k_sz == 0) {
        return NULL;
    }

    unsigned char* r = malloc(sizeof(unsigned char) * s_sz);
    if (r == NULL) {
        return NULL;
    }

    for (size_t i = 0, j = 0; i < s_sz; ++i) {
        r[i] = s[i] ^ key[j];
        j = (j + 1) % k_sz;
    }
    return r;
}

unsigned char *repeating_key_xor_s(const char* s, const char* key) {
    return repeating_key_xor((unsigned char*)s, strlen(s), (unsigned char*)key, strlen(key));
}

int hamming_distance_s(const char *a, const char *b) {
    size_t sz = strlen(a);
    if (sz != strlen(b)) {
        return -1;
    }

    return hamming_distance((const unsigned char *)a, (const unsigned char *)b, sz);
}

int hamming_distance(const unsigned char *a, const unsigned char *b, size_t sz) {
    int hamming = 0;
    for (size_t i = 0; i < sz; ++i) {
        if (a[i] == b[i]) {
            continue;
        }
        unsigned char c = a[i] ^ b[i];
        int count = 0;
        for (; c; count++) {
            c &= c - 1;
        }
        hamming += count;
    }

    return hamming;
}
