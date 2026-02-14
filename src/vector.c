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
    vec->capacity = new_cap;
    vec->elements = reallocarray(vec->elements, new_cap, sizeof(void *));

    if (vec->elements == NULL) {
        return -1;
    }
    return 0;
}

static int vec_grow(Vector *const vec) {
    size_t new_cap;
    if (vec->capacity == 0) {
        new_cap = VEC_INIT_CAP; ;
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

void vec_clear(Vector *vec) {
    vec_destroy(vec);
    vec_init(vec, vec->destroy);
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

    if (vec_len(vec) + 1 >= vec->capacity) {
        if (vec_grow(vec) != 0) {
            return -1;
        }
    }

    if (index > vec_len(vec)) {
        return -1;
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

    vec->capacity = vec_len(vec);

#if !defined(__OpenBSD__)
    vec->elements = reallocf(vec->elements, sizeof(void *) * vec->capacity);
#else
    vec->elements = reallocarray(vec->elements, vec->capacity, sizeof(void *));
#endif

    if (vec->elements == NULL) {
        return -1;
    }

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
