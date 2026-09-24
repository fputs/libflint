#include "lfhashmap.h"
#include "lfcompat.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum { EMPTY, LIVE, TOMB };

static int valid(const LfHashMap *m) {
    return m && m->cap && m->entries && m->match && m->hash;
}

static size_t locate(const LfHashMap *m, const void *key) {
    size_t i = m->hash(key) & (m->cap - 1);
    for (size_t n = 0; n < m->cap; n++, i = (i + 1) & (m->cap - 1)) {
        if (m->entries[i].state == EMPTY) break;
        if (m->entries[i].state == LIVE && !m->match(m->entries[i].key, key))
            return i;
    }
    return SIZE_MAX;
}

static int rehash(LfHashMap *m, size_t cap) {
    LfHashMapEntry *entries = lf_reallocarray(NULL, cap, sizeof(*entries));
    if (!entries) return -1;
    memset(entries, 0, cap * sizeof(*entries));
    for (size_t i = 0; i < m->cap; i++) {
        if (m->entries[i].state != LIVE) continue;
        size_t j = m->hash(m->entries[i].key) & (cap - 1);
        while (entries[j].state == LIVE) j = (j + 1) & (cap - 1);
        entries[j] = m->entries[i];
    }
    free(m->entries);
    m->entries = entries;
    m->cap = cap;
    m->tombs = 0;
    return 0;
}

int lf_hashmap_init(LfHashMap *m, size_t cap,
                    int (*match)(const void *, const void *),
                    size_t (*hash)(const void *),
                    void (*dk)(void *), void (*dv)(void *)) {
    if (!m) return -1;
    memset(m, 0, sizeof(*m));
    if (!match || !hash) return -1;
    size_t n = 8;
    while (n < cap) {
        if (n > SIZE_MAX / 2) return -1;
        n *= 2;
    }
    m->match = match;
    m->hash = hash;
    m->destroy_key = dk;
    m->destroy_value = dv;
    return rehash(m, n);
}

int lf_hashmap_insert(LfHashMap *m, void *key, void *value) {
    if (!valid(m)) return -1;
    if (locate(m, key) != SIZE_MAX) return 1;
    if (m->len >= m->cap - m->cap / 4) {
        if (m->cap > SIZE_MAX / 2 || rehash(m, m->cap * 2)) return -1;
    } else if (m->len + m->tombs >= m->cap - 1 && rehash(m, m->cap)) {
        return -1;
    }
    size_t i = m->hash(key) & (m->cap - 1);
    while (m->entries[i].state == LIVE) i = (i + 1) & (m->cap - 1);
    if (m->entries[i].state == TOMB) m->tombs--;
    m->entries[i] = (LfHashMapEntry){key, value, LIVE};
    m->len++;
    return 0;
}

int lf_hashmap_replace(LfHashMap *m, void *key, void *value) {
    if (!valid(m)) return -1;
    size_t i = locate(m, key);
    if (i == SIZE_MAX) return 1;
    LfHashMapEntry *e = &m->entries[i];
    if (m->destroy_key && e->key != key) m->destroy_key(e->key);
    if (m->destroy_value && e->value != value) m->destroy_value(e->value);
    e->key = key;
    e->value = value;
    return 0;
}

int lf_hashmap_find(const LfHashMap *m, const void *key, void **value) {
    if (!valid(m) || !value) return -1;
    size_t i = locate(m, key);
    if (i == SIZE_MAX) return 1;
    *value = m->entries[i].value;
    return 0;
}

int lf_hashmap_remove(LfHashMap *m, const void *key, void **stored_key, void **value) {
    if (!valid(m) || !stored_key || !value || stored_key == value) return -1;
    size_t i = locate(m, key);
    if (i == SIZE_MAX) return 1;
    *stored_key = m->entries[i].key;
    *value = m->entries[i].value;
    m->entries[i] = (LfHashMapEntry){NULL, NULL, TOMB};
    m->len--;
    m->tombs++;
    return 0;
}

void lf_hashmap_clear(LfHashMap *m) {
    if (!m) return;
    for (size_t i = 0; i < m->cap; i++) {
        if (m->entries[i].state != LIVE) continue;
        if (m->destroy_key) m->destroy_key(m->entries[i].key);
        if (m->destroy_value) m->destroy_value(m->entries[i].value);
    }
    if (m->entries) memset(m->entries, 0, m->cap * sizeof(*m->entries));
    m->len = m->tombs = 0;
}

void lf_hashmap_destroy(LfHashMap *m) {
    if (!m) return;
    lf_hashmap_clear(m);
    free(m->entries);
    memset(m, 0, sizeof(*m));
}
