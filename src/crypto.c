#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lfcodec.h"
#include "lfpuzzle.h"

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

static int b64_encoded_len(size_t n, size_t *need) {
    size_t groups = n / 3 + (n % 3 != 0);
    if (groups > (SIZE_MAX - 1) / 4) {
        return -1;
    }
    *need = groups * 4 + 1;
    return 0;
}

static void b64_encode_triplet(const unsigned char t[3], char out[4]) {
    out[0] = b64_table[(t[0] & 0xfc) >> 2];
    out[1] = b64_table[((t[0] & 0x03) << 4) | ((t[1] & 0xf0) >> 4)];
    out[2] = b64_table[((t[1] & 0x0f) << 2) | ((t[2] & 0xc0) >> 6)];
    out[3] = b64_table[t[2] & 0x3f];
}

int b64_encode_into(const unsigned char *s, size_t sz, char *dest, size_t dest_cap) {
    size_t need;
    if (dest == NULL || (s == NULL && sz != 0)) {
        return -1;
    }
    if (b64_encoded_len(sz, &need) != 0 || dest_cap < need) {
        return -1;
    }

    size_t o = 0;
    size_t i = 0;
    while (i + 3 <= sz) {
        b64_encode_triplet(s + i, dest + o);
        i += 3;
        o += 4;
    }
    if (i < sz) {
        unsigned char t[3] = {s[i], 0, 0};
        if (sz - i == 2) {
            t[1] = s[i + 1];
        }
        char q[4];
        b64_encode_triplet(t, q);
        dest[o++] = q[0];
        dest[o++] = q[1];
        if (sz - i == 2) {
            dest[o++] = q[2];
            dest[o++] = '=';
        } else {
            dest[o++] = '=';
            dest[o++] = '=';
        }
    }
    dest[o] = '\0';
    return 0;
}

char *b64_encode(const unsigned char *s, size_t sz) {
    size_t need;
    if (b64_encoded_len(sz, &need) != 0) {
        return NULL;
    }
    char *out = malloc(need);
    if (out == NULL) {
        return NULL;
    }
    if (b64_encode_into(s, sz, out, need) != 0) {
        free(out);
        return NULL;
    }
    return out;
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

int b64_decode_into(const char *s, size_t sz, unsigned char *dest, size_t dest_cap,
                    size_t *decode_sz) {
    if ((s == NULL && sz != 0) || dest == NULL) {
        return -1;
    }

    int i = 0;
    size_t size = 0;
    unsigned char buf[3];
    unsigned char tmp[4];
    size_t n = 0;

    while (n < sz) {
        unsigned char c = (unsigned char)s[n];
        if (c == '=') {
            break;
        }
        if (b64_val(c) < 0) {
            return -1;
        }

        tmp[i++] = c;
        n++;

        if (i == 4) {
            if (b64_decode_quartet(tmp, buf) != 0) {
                return -1;
            }
            if (dest_cap - size < 3) {
                return -1;
            }
            dest[size++] = buf[0];
            dest[size++] = buf[1];
            dest[size++] = buf[2];
            i = 0;
        }
    }

    if (n < sz && (unsigned char)s[n] == '=') {
        if (i != 2 && i != 3) {
            return -1;
        }
        size_t pads = 4 - (size_t)i;
        for (size_t p = 0; p < pads; ++p) {
            if (n >= sz || (unsigned char)s[n] != '=') {
                return -1;
            }
            n++;
        }
        if (n != sz) {
            return -1;
        }
    } else if (i == 1) {
        return -1;
    }

    if (i > 0) {
        for (int j = i; j < 4; ++j) {
            tmp[j] = 'A';
        }
        if (b64_decode_quartet(tmp, buf) != 0) {
            return -1;
        }
        size_t extra = (size_t)(i - 1);
        if (dest_cap - size < extra) {
            return -1;
        }
        for (int j = 0; j < i - 1; ++j) {
            dest[size++] = buf[j];
        }
    }

    if (size < dest_cap) {
        dest[size] = '\0';
    }
    if (decode_sz != NULL) {
        *decode_sz = size;
    }
    return 0;
}

unsigned char *b64_decode(const char *s, size_t sz, size_t *decode_sz) {
    if (s == NULL && sz != 0) {
        return NULL;
    }
    size_t cap = sz + 1;
    if (sz > SIZE_MAX - 1) {
        return NULL;
    }
    unsigned char *out = malloc(cap);
    if (out == NULL) {
        return NULL;
    }
    size_t got = 0;
    if (b64_decode_into(s, sz, out, cap, &got) != 0) {
        free(out);
        return NULL;
    }
    if (decode_sz != NULL) {
        *decode_sz = got;
    }
    return out;
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

static int hex_decoded_len(const char *orig, const char **body, size_t *out_sz, int *pad) {
    const char *sptr = orig;
    if (sptr[0] == '0' && (sptr[1] == 'x' || sptr[1] == 'X')) {
        sptr += 2;
    }

    size_t slen = strlen(sptr);
    int odd = (slen % 2 != 0);
    *body = sptr;
    *pad = odd;
    *out_sz = (slen + (size_t)odd) / 2;
    return 0;
}

int hex_decode_into(const char *src, unsigned char *dest, size_t dest_cap, size_t *out_sz) {
    if (src == NULL || dest == NULL) {
        return -1;
    }

    const char *sptr;
    int pad;
    size_t need;
    if (hex_decoded_len(src, &sptr, &need, &pad) != 0) {
        return -1;
    }
    if (dest_cap < need) {
        return -1;
    }

    size_t slen = strlen(sptr);
    size_t out_i = 0;
    size_t in_i = 0;
    if (pad) {
        int lo = hex_nibble(sptr[0]);
        if (lo < 0) {
            return -1;
        }
        dest[out_i++] = (unsigned char)lo;
        in_i = 1;
    }

    for (; in_i < slen; in_i += 2) {
        int hi = hex_nibble(sptr[in_i]);
        int lo = hex_nibble(sptr[in_i + 1]);
        if (hi < 0 || lo < 0) {
            return -1;
        }
        dest[out_i++] = (unsigned char)((hi << 4) | lo);
    }

    if (out_sz != NULL) {
        *out_sz = need;
    }
    return 0;
}

unsigned char *hex_decode(const char *orig, size_t *sz) {
    if (orig == NULL || sz == NULL) {
        return NULL;
    }

    const char *body;
    int pad;
    size_t out_sz;
    if (hex_decoded_len(orig, &body, &out_sz, &pad) != 0) {
        return NULL;
    }

    unsigned char *hex = malloc(out_sz == 0 ? 1 : out_sz);
    if (hex == NULL) {
        return NULL;
    }
    if (hex_decode_into(orig, hex, out_sz == 0 ? 1 : out_sz, &out_sz) != 0) {
        free(hex);
        return NULL;
    }
    *sz = out_sz;
    return hex;
}

int hex_encode_into(const unsigned char *src, size_t n, char *dest, size_t dest_cap) {
    if (n > (SIZE_MAX - 1) / 2) {
        return -1;
    }
    if ((src == NULL && n != 0) || dest == NULL) {
        return -1;
    }
    size_t need = n * 2 + 1;
    if (dest_cap < need) {
        return -1;
    }

    for (size_t i = 0; i < n; ++i) {
        snprintf(dest + i * 2, 3, "%02x", src[i]);
    }
    dest[need - 1] = '\0';
    return 0;
}

char *hex_encode(const unsigned char *hex, size_t sz) {
    if (sz > (SIZE_MAX - 1) / 2) {
        return NULL;
    }
    if (hex == NULL && sz != 0) {
        return NULL;
    }
    size_t need = sz * 2 + 1;
    char *s = malloc(need);
    if (s == NULL) {
        return NULL;
    }
    if (hex_encode_into(hex, sz, s, need) != 0) {
        free(s);
        return NULL;
    }
    return s;
}

char *hex_to_str(const unsigned char *hex, size_t sz) {
    if (sz == SIZE_MAX || (hex == NULL && sz != 0)) {
        return NULL;
    }
    char *s = malloc(sz + 1);
    if (s == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < sz; ++i) {
        s[i] = (char)hex[i];
    }
    s[sz] = '\0';
    return s;
}

unsigned char *repeating_key_xor(const unsigned char *s, size_t s_sz,
                                 const unsigned char *key, size_t k_sz) {
    if (k_sz == 0 || (s == NULL && s_sz != 0) || key == NULL) {
        return NULL;
    }

    unsigned char *r = malloc(s_sz == 0 ? 1 : s_sz);
    if (r == NULL) {
        return NULL;
    }

    for (size_t i = 0, j = 0; i < s_sz; ++i) {
        r[i] = s[i] ^ key[j];
        j = (j + 1) % k_sz;
    }
    return r;
}

unsigned char *repeating_key_xor_s(const char *s, const char *key) {
    if (s == NULL || key == NULL) {
        return NULL;
    }
    return repeating_key_xor((const unsigned char *)s, strlen(s),
                             (const unsigned char *)key, strlen(key));
}

int hamming_distance_s(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return -1;
    }
    size_t sz = strlen(a);
    if (sz != strlen(b)) {
        return -1;
    }

    return hamming_distance((const unsigned char *)a, (const unsigned char *)b, sz);
}

int hamming_distance(const unsigned char *a, const unsigned char *b, size_t sz) {
    if ((a == NULL || b == NULL) && sz != 0) {
        return -1;
    }
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
        if (hamming > INT_MAX - count) {
            return -1;
        }
        hamming += count;
    }

    return hamming;
}

static int ses_score_sw(char c) {
    switch (c) {
        case 'e': case 'E': return 13;
        case 't': case 'T': return 12;
        case 'a': case 'A': return 11;
        case 'o': case 'O': return 10;
        case 'i': case 'I': return 9;
        case 'n': case 'N': return 8;
        case ' ': return 7;
        case 's': case 'S': return 6;
        case 'h': case 'H': return 5;
        case 'r': case 'R': return 4;
        case 'd': case 'D': return 3;
        case 'l': case 'L': return 2;
        case 'u': case 'U': return 1;
        default: return 0;
    }
}

int simple_english_scoring(const char *s) {
    if (s == NULL) {
        return -1;
    }
    int score = 0;
    for (const char *c = s; *c != '\0'; ++c) {
        int weight = ses_score_sw(*c);
        if (score > INT_MAX - weight) {
            return -1;
        }
        score += weight;
    }
    return score;
}
