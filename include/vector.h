#ifndef LIBFLINT_H_VECTOR
#define LIBFLINT_H_VECTOR

#include <memory.h>

typedef struct Vector {
    size_t capacity;
    size_t length;
    void **elements;
    void (*destroy)(void *data);
} Vector;

int vec_init(Vector *vec, void (*destroy)(void *data));

int vec_init_with_capacity(Vector *vec, void (*destroy)(void *data), size_t cap);

void vec_destroy(Vector *vec);

int vec_insert(Vector *vec, void *data, size_t index);

int vec_push(Vector *vec, void *data);

void *vec_safe_at(Vector *vec, size_t index);

void *vec_remove(Vector *vec, size_t index);

#define vec_at(v, i) (v)->elements[(i)]

#define vec_len(v) (v)->length

#endif // LIBFLINT_H_VECTOR
