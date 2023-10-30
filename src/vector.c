#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__
#include <bsd/stdlib.h>
#endif

#include "vector.h"

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

static int vec_grow(Vector *const vec) {
    vec->capacity *= 2;

#ifdef __OpenBSD__
    vec->elements = reallocarray(vec->elements, vec->capacity, sizeof(void *));
#else
    vec->elements = reallocf(vec->elements, sizeof(void *) * vec->capacity);
#endif

    if (vec->elements == NULL) {
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
}

int vec_insert(Vector *vec, void *data, size_t index) {
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

#ifdef __OpenBSD__
    vec->elements = reallocarray(vec->elements, vec->capacity, sizeof(void *));
#else
    vec->elements = reallocf(vec->elements, sizeof(void *) * vec->capacity);
#endif

    if (vec->elements == NULL) {
        return -1;
    }

    return 0;
}
