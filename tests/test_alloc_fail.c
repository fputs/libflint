#include "lfbitset.h"
#include "lfhashmap.h"
#include "lfheap.h"
#include "lfstrbuf.h"
#include "lftest.h"
#include <errno.h>
#include <stdlib.h>

/* Fail exactly one allocation after the requested number of successes.
 * Only the module copies built for this test call these wrappers. */
static int fail_after = -1;
static int failures;

static int should_fail(void) {
    if (fail_after < 0) return 0;
    if (fail_after-- != 0) return 0;
    failures++;
    errno = ENOMEM;
    return 1;
}

void *lf_test_malloc(size_t size) {
    return should_fail() ? NULL : malloc(size);
}

void *lf_test_realloc(void *ptr, size_t size) {
    return should_fail() ? NULL : realloc(ptr, size);
}

static int compare(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static size_t hash(const void *p) { return (size_t)*(const int *)p; }
static int destroyed;
static void destroy(void *p) { (void)p; destroyed++; }

static void test_hashmap(void) {
    LfHashMap m;
    fail_after = 0;
    ASSERT_EQ(lf_hashmap_init(&m, 8, compare, hash, destroy, destroy), -1);
    lf_hashmap_destroy(&m);
    ASSERT_EQ(destroyed, 0);
    ASSERT_EQ(lf_hashmap_init(&m, 8, compare, hash, destroy, destroy), 0);
    int keys[8], values[8];
    for (int i = 0; i < 8; i++) { keys[i] = i; values[i] = 100 + i; }
    for (int i = 0; i < 6; i++)
        ASSERT_EQ(lf_hashmap_insert(&m, &keys[i], &values[i]), 0);
    LfHashMapEntry *entries = m.entries;
    fail_after = 0;
    ASSERT_EQ(lf_hashmap_insert(&m, &keys[6], &values[6]), -1);
    ASSERT_EQ(m.entries, entries);
    ASSERT_EQ(m.cap, 8);
    ASSERT_EQ(m.len, 6);
    ASSERT_EQ(destroyed, 0);
    for (int i = 0; i < 6; i++) {
        void *value = NULL;
        ASSERT_EQ(lf_hashmap_find(&m, &keys[i], &value), 0);
        ASSERT_EQ(value, &values[i]);
    }
    void *value = NULL;
    ASSERT_EQ(lf_hashmap_find(&m, &keys[6], &value), 1);
    ASSERT_EQ(lf_hashmap_insert(&m, &keys[6], &values[6]), 0);
    lf_hashmap_destroy(&m);
    ASSERT_EQ(destroyed, 14);

    /* Build tombstones without growing, then fail same-capacity rehash. */
    destroyed = 0;
    ASSERT_EQ(lf_hashmap_init(&m, 8, compare, hash, destroy, destroy), 0);
    for (int i = 0; i < 7; i++) {
        void *key = NULL;
        ASSERT_EQ(lf_hashmap_insert(&m, &keys[i], &values[i]), 0);
        if (i < 6) {
            ASSERT_EQ(lf_hashmap_remove(&m, &keys[i], &key, &value), 0);
            ASSERT_EQ(key, &keys[i]);
            ASSERT_EQ(value, &values[i]);
        }
    }
    entries = m.entries;
    fail_after = 0;
    ASSERT_EQ(lf_hashmap_insert(&m, &keys[7], &values[7]), -1);
    ASSERT_EQ(m.entries, entries);
    ASSERT_EQ(m.cap, 8);
    ASSERT_EQ(m.len, 1);
    ASSERT_EQ(m.tombs, 6);
    ASSERT_EQ(destroyed, 0);
    ASSERT_EQ(lf_hashmap_find(&m, &keys[6], &value), 0);
    ASSERT_EQ(value, &values[6]);
    ASSERT_EQ(lf_hashmap_find(&m, &keys[7], &value), 1);
    ASSERT_EQ(lf_hashmap_insert(&m, &keys[7], &values[7]), 0);
    ASSERT_EQ(m.tombs, 0);
    lf_hashmap_destroy(&m);
    ASSERT_EQ(destroyed, 4);
}

static void test_strbuf(void) {
    LfStrBuf b;
    fail_after = 0;
    ASSERT_EQ(lf_strbuf_init(&b), -1);
    lf_strbuf_destroy(&b);
    ASSERT_EQ(lf_strbuf_append(&b, "123456789"), 0);
    LfStrBuf old = b;
    fail_after = 0;
    ASSERT_EQ(lf_strbuf_reserve(&b, b.cap * 2), -1);
    ASSERT_EQ(b.data, old.data);
    ASSERT_EQ(b.len, old.len);
    ASSERT_EQ(b.cap, old.cap);
    ASSERT_STR_EQ(b.data, "123456789");

    fail_after = 0;
    ASSERT_EQ(lf_strbuf_append(&b, b.data), -1);
    ASSERT_EQ(b.data, old.data);
    ASSERT_EQ(b.len, old.len);
    ASSERT_EQ(b.cap, old.cap);
    ASSERT_STR_EQ(b.data, "123456789");

    /* Fail the formatting temporary, then the destination growth. */
    for (int allocation = 0; allocation < 2; allocation++) {
        fail_after = allocation;
        ASSERT_EQ(lf_strbuf_appendf(&b, "%s", b.data), -1);
        ASSERT_EQ(b.data, old.data);
        ASSERT_EQ(b.len, old.len);
        ASSERT_EQ(b.cap, old.cap);
        ASSERT_STR_EQ(b.data, "123456789");
    }
    ASSERT_EQ(lf_strbuf_appendf(&b, "%s", b.data), 0);
    ASSERT_STR_EQ(b.data, "123456789123456789");
    lf_strbuf_destroy(&b);
    fail_after = 0;
    ASSERT_NULL(lf_strbuf_detach(&b));
    ASSERT_NULL(b.data);
    ASSERT_EQ(b.len, 0);
    ASSERT_EQ(b.cap, 0);
    char *text = lf_strbuf_detach(&b);
    ASSERT_STR_EQ(text, "");
    free(text);
    lf_strbuf_destroy(&b);
}

static void test_heap(void) {
    LfHeap h;
    destroyed = 0;
    ASSERT_EQ(lf_heap_init(&h, compare, destroy), 0);
    ASSERT_EQ(lf_heap_reserve(&h, 2), 0);
    int values[] = {3, 2, 1};
    ASSERT_EQ(lf_heap_push(&h, &values[0]), 0);
    ASSERT_EQ(lf_heap_push(&h, &values[1]), 0);
    void **data = h.data;
    fail_after = 0;
    ASSERT_EQ(lf_heap_push(&h, &values[2]), -1);
    ASSERT_EQ(h.data, data);
    ASSERT_EQ(h.cap, 2);
    ASSERT_EQ(h.len, 2);
    ASSERT_EQ(destroyed, 0);
    void *value = NULL;
    ASSERT_EQ(lf_heap_peek(&h, &value), 0);
    ASSERT_EQ(value, &values[1]);
    ASSERT_EQ(lf_heap_push(&h, &values[2]), 0);
    for (int i = 2; i >= 0; i--) {
        ASSERT_EQ(lf_heap_pop(&h, &value), 0);
        ASSERT_EQ(value, &values[i]);
    }
    lf_heap_destroy(&h);
    ASSERT_EQ(destroyed, 0);
}

static void test_bitset(void) {
    LfBitSet s;
    fail_after = 0;
    ASSERT_EQ(lf_bitset_init(&s, 65), -1);
    ASSERT_NULL(s.words);
    ASSERT_EQ(s.nbits, 0);
    ASSERT_EQ(s.nwords, 0);
    lf_bitset_destroy(&s);
    ASSERT_EQ(lf_bitset_init(&s, 65), 0);
    ASSERT_EQ(lf_bitset_set(&s, 64), 0);
    ASSERT_EQ(lf_bitset_count(&s), 1);
    lf_bitset_destroy(&s);
}

int main(void) {
    test_hashmap();
    test_strbuf();
    test_heap();
    test_bitset();
    ASSERT_EQ(failures, 11);
    TEST_REPORT();
}
