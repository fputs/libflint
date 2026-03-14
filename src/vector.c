#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__
#include <bsd/stdlib.h>
#endif

#if defined(__APPLE__) || defined(__MACH__)
#include "lfmacos.h"
#endif

#include "lfvector.h"

#define VEC_INIT_CAP 2

int vec_init(Vector *vec, void (*destroy)(void *data)) {
    return vec_init_with_capacity(vec, destroy, VEC_INIT_CAP);
}

int vec_init_with_capacity(Vector *vec, void (*destroy)(void *data), size_t capacity) {
    if (vec == NULL) {
        return -1;
    }
    vec->destroy = destroy;
    vec_len(vec) = 0;
    vec->capacity = capacity;
    vec->elements = malloc(sizeof (void*) * capacity);
    if (vec->elements == NULL) {
        return -1;
    }
    memset(vec->elements, 0, vec->capacity * sizeof(void *));

    return 0;
}

static int vec_expand(Vector *vec, size_t new_cap) {
    void *tmp = reallocarray(vec->elements, new_cap, sizeof(void *));
    if (tmp == NULL) {
        return -1;
    }
    vec->elements = tmp;
    vec->capacity = new_cap;
    return 0;
}

static int vec_grow(Vector *const vec) {
    size_t new_cap;
    if (vec->capacity == 0) {
        new_cap = VEC_INIT_CAP;
    } else {
        new_cap = vec->capacity * 2;
    }
    return vec_expand(vec, new_cap);
}

int vec_grow_to(Vector *vec, const size_t new_cap) {
    if (vec->capacity > new_cap) {
        return -1;
    }
    return vec_expand(vec, new_cap);
}

int vec_clear(Vector *vec) {
    void (*destroy)(void *data) = vec->destroy;
    vec_destroy(vec);
    if (vec_init(vec, destroy) != 0) {
        return -1;
    }
    return 0;
}

void vec_destroy(Vector *vec) {
    if (vec->destroy) {
        for (size_t i = 0; i < vec_len(vec); ++i) {
            vec->destroy(vec->elements[i]);
        }
    }
    free(vec->elements);
}

int vec_insert(Vector *vec, void *data, size_t index) {
    if (index > vec_len(vec)) {
        return -1;
    }

    if (vec_len(vec) + 1 > vec->capacity) {
        if (vec_grow(vec) != 0) {
            return -1;
        }
    }

    if (index < vec_len(vec)) {
        void *a = vec->elements[index];
        vec->elements[index] = data;

        void *b = NULL;
        for (size_t i = index + 1; i <= vec_len(vec); ++i) {
            b = vec->elements[i];
            vec->elements[i] = a;
            a = b;
        }
    } else /* index == vec_len(vec) */ {
        vec->elements[index] = data;
    }

    vec_len(vec) += 1;
    return 0;
}

int vec_push(Vector *vec, void *data) {
    return vec_insert(vec, data, vec_len(vec));
}

void *vec_safe_at(Vector *vec, size_t index) {
    if (index >= vec_len(vec)) {
        return NULL;
    }
    return vec->elements[index];
}

void *vec_remove(Vector *vec, size_t index) {
    if (index >= vec_len(vec)) {
        return NULL;
    }

    void *r = vec_at(vec, index);
    for (size_t i = index + 1; i < vec_len(vec); ++i) {
        vec->elements[i - 1] = vec->elements[i];
    }
    vec->elements[vec_len(vec) - 1] = NULL;
    vec->length -= 1;
    return r;
}

int vec_shrink(Vector *vec) {
    if (vec_len(vec) == vec_cap(vec)) {
        return 0;
    }

    if (vec_len(vec) == 0) {
        free(vec->elements);
        vec->elements = NULL;
        vec->capacity = 0;
        return 0;
    }

    size_t new_cap = vec_len(vec);

#if !defined(__OpenBSD__)
    void *tmp = reallocf(vec->elements, sizeof(void *) * new_cap);
#else
    void *tmp = reallocarray(vec->elements, new_cap, sizeof(void *));
#endif

    if (tmp == NULL) {
        return -1;
    }

    vec->elements = tmp;
    vec->capacity = new_cap;

    return 0;
}

const void *vec_min(const Vector *vec, int(*cmp)(const void *a, const void *b)) {
    if (vec_len(vec) == 0) {
        return NULL;
    }
    void *a = vec->elements[0];
    for (size_t i = 1; i < vec_len(vec); ++i) {
        if (cmp(a, vec->elements[i]) > 0) {
            a = vec->elements[i];
        }
    }
    return a;
}

const void *vec_max(const Vector *vec, int(*cmp)(const void *a, const void *b)) {
    if (vec_len(vec) == 0) {
        return NULL;
    }
    void *a = vec->elements[0];
    for (size_t i = 1; i < vec_len(vec); ++i) {
        if (cmp(a, vec->elements[i]) < 0) {
            a = vec->elements[i];
        }
    }
    return a;
}

void *vec_pop(Vector *vec) {
    if (vec_len(vec) == 0) {
        return NULL;
    }
    return vec_remove(vec, vec_len(vec) - 1);
}

void vec_reverse(Vector *vec) {
    size_t lo = 0;
    size_t hi = vec_len(vec);
    while (lo + 1 < hi) {
        --hi;
        void *tmp = vec->elements[lo];
        vec->elements[lo] = vec->elements[hi];
        vec->elements[hi] = tmp;
        ++lo;
    }
}

#if defined(__APPLE__)
static int vec_sort_cmp_wrapper(void *thunk, const void *a, const void *b) {
    int (*cmp)(const void *, const void *) = thunk;
    const void *ea = *(const void **)a;
    const void *eb = *(const void **)b;
    return cmp(ea, eb);
}
#else
static int vec_sort_cmp_wrapper(const void *a, const void *b, void *thunk) {
    int (*cmp)(const void *, const void *) = thunk;
    const void *ea = *(const void **)a;
    const void *eb = *(const void **)b;
    return cmp(ea, eb);
}
#endif

void vec_sort(Vector *vec, int (*cmp)(const void *a, const void *b)) {
    if (vec_len(vec) < 2) {
        return;
    }
#if defined(__APPLE__)
    qsort_r(vec->elements, vec_len(vec), sizeof(void *), (void *)cmp, vec_sort_cmp_wrapper);
#else
    qsort_r(vec->elements, vec_len(vec), sizeof(void *), vec_sort_cmp_wrapper, (void *)cmp);
#endif
}

void *vec_bsearch(Vector *vec, const void *key, int (*cmp)(const void *a, const void *b)) {
    size_t lo = 0;
    size_t hi = vec_len(vec);
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int r = cmp(key, vec->elements[mid]);
        if (r == 0) {
            return vec->elements[mid];
        } else if (r < 0) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    return NULL;
}

int vec_cmp_int(const void *a, const void *b) {
    const int x = *(int*)a;
    const int y = *(int*)b;

    if (x > y) {
        return 1;
    }
    if (x < y) {
        return -1;
    }
    return 0;
}

int vec_cmp_char(const void *a, const void *b) {
    const char x = *(char*)a;
    const char y = *(char*)b;

    if (x > y) {
        return 1;
    }
    if (x < y) {
        return -1;
    }
    return 0;
}
