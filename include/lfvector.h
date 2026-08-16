#ifndef LIBFLINT_H_VECTOR
#define LIBFLINT_H_VECTOR

#include <string.h>

typedef struct Vector {
    size_t capacity;
    size_t length;
    void **elements;
    void (*destroy)(void *data);
} Vector;

int vec_init(Vector *vec, void (*destroy)(void *data));

int vec_init_with_capacity(Vector *vec, void (*destroy)(void *data), size_t cap);

int vec_clear(Vector *vec);

void vec_destroy(Vector *vec);

int vec_grow_to(Vector *vec, const size_t new_cap);

int vec_reserve(Vector *vec, size_t n);

int vec_insert(Vector *vec, void *data, size_t index);

int vec_push(Vector *vec, void *data);

void *vec_safe_at(Vector *vec, size_t index);

void *vec_remove(Vector *vec, size_t index);

int vec_shrink(Vector *vec);

const void *vec_min(const Vector *vec, int(*cmp)(const void *a, const void *b));

const void *vec_max(const Vector *vec, int(*cmp)(const void *a, const void *b));

void *vec_pop(Vector *vec);

void vec_reverse(Vector *vec);

void vec_sort(Vector *vec, int (*cmp)(const void *a, const void *b));

void *vec_bsearch(Vector *vec, const void *key, int (*cmp)(const void *a, const void *b));

int vec_cmp_int(const void *a, const void *b);

int vec_cmp_char(const void *a, const void *b);

#define vec_at(v, i) (v)->elements[(i)]

#define vec_len(v) (v)->length

#define vec_cap(v) (v)->capacity

#endif // LIBFLINT_H_VECTOR
