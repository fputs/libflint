#include <stdlib.h>

#include "lfringbuf.h"

int rb_init(RingBuffer *rb, size_t capacity, void (*destroy)(void *)) {
   if (rb == NULL || capacity == 0) {
      return -1;
   }

   if ((rb->buf = malloc(sizeof(void*) * capacity)) == NULL) {
      return -1;
   }

   rb->capacity = capacity;
   rb->destroy = destroy;
   rb->count = 0;
   rb->head = 0;
   rb->tail = 0;

   return 0;
}

void rb_destroy(RingBuffer *rb) {
   rb_clear(rb);
   free(rb->buf);
   rb->buf = NULL;
}

int rb_push(RingBuffer *rb, void *data) {
   if (rb_is_full(rb)) {
      return -1;
   }
   rb->buf[rb->tail] = data;
   rb->tail = (rb->tail + 1) % rb->capacity;
   rb->count++;
   return 0;
}

int rb_pop(RingBuffer *rb, void **data) {
   if (rb_is_empty(rb)) {
      return -1;
   }
   *data = rb->buf[rb->head];
   rb->head = (rb->head + 1) % rb->capacity;
   rb->count--;
   return 0;
}

void *rb_peek(RingBuffer *rb) {
   if (rb_is_empty(rb)) {
      return NULL;
   }
   return rb->buf[rb->head];
}

void rb_clear(RingBuffer *rb) {
   for (size_t i = 0; i < rb->count; i++) {
      const size_t idx = (rb->head + i) % rb->capacity;
      if (rb->destroy != NULL) {
         rb->destroy(rb->buf[idx]);
      }
      rb->buf[idx] = NULL;
   }

   rb->head = 0;
   rb->tail = 0;
   rb->count = 0;
}
