#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lfcompat.h"
#include "lfhashset.h"

enum {
    HS_EMPTY = 0,
    HS_LIVE = 1,
    HS_TOMB = 2
};

static size_t hs_pow2(size_t n) {
    size_t c = 8;
    if (n <= c) {
        return c;
    }
    while (c < n) {
        if (c > SIZE_MAX / 2) {
            return 0;
        }
        c *= 2;
    }
    return c;
}

static int hs_rehash(LfHashSet *set, size_t ncap) {
    void **slots = lf_reallocarray(NULL, ncap, sizeof(void *));
    unsigned char *state = calloc(ncap, 1);
    if (slots == NULL || state == NULL) {
        free(slots);
        free(state);
        return -1;
    }
    memset(slots, 0, ncap * sizeof(void *));

    size_t mask = ncap - 1;
    for (size_t i = 0; i < set->cap; i++) {
        if (set->state[i] != HS_LIVE) {
            continue;
        }
        size_t j = set->hash(set->slots[i]) & mask;
        while (state[j] == HS_LIVE) {
            j = (j + 1) & mask;
        }
        slots[j] = set->slots[i];
        state[j] = HS_LIVE;
    }

    free(set->slots);
    free(set->state);
    set->slots = slots;
    set->state = state;
    set->cap = ncap;
    set->tombs = 0;
    return 0;
}

int lf_hashset_init(LfHashSet *set, size_t cap,
                    int (*match)(const void *a, const void *b),
                    size_t (*hash)(const void *data),
                    void (*destroy)(void *data)) {
    if (set == NULL || match == NULL || hash == NULL) {
        return -1;
    }

    size_t ncap = hs_pow2(cap);
    if (ncap == 0) {
        return -1;
    }

    void **slots = lf_reallocarray(NULL, ncap, sizeof(void *));
    unsigned char *state = calloc(ncap, 1);
    if (slots == NULL || state == NULL) {
        free(slots);
        free(state);
        return -1;
    }
    memset(slots, 0, ncap * sizeof(void *));

    set->slots = slots;
    set->state = state;
    set->cap = ncap;
    set->len = 0;
    set->tombs = 0;
    set->match = match;
    set->hash = hash;
    set->destroy = destroy;
    return 0;
}

void lf_hashset_destroy(LfHashSet *set) {
    if (set == NULL) {
        return;
    }
    if (set->destroy != NULL && set->state != NULL) {
        for (size_t i = 0; i < set->cap; i++) {
            if (set->state[i] == HS_LIVE) {
                set->destroy(set->slots[i]);
            }
        }
    }
    free(set->slots);
    free(set->state);
    memset(set, 0, sizeof(*set));
}

static int hs_grow_if_needed(LfHashSet *set) {
    if (set->cap == 0) {
        return -1;
    }
    if ((set->len + 1) * 4 <= set->cap * 3 &&
        set->len + set->tombs + 1 < set->cap) {
        return 0;
    }
    if (set->cap > SIZE_MAX / 2) {
        return -1;
    }
    return hs_rehash(set, set->cap * 2);
}

int lf_hashset_insert(LfHashSet *set, const void *data) {
    if (set == NULL || set->match == NULL || set->hash == NULL) {
        return -1;
    }
    if (hs_grow_if_needed(set) != 0) {
        return -1;
    }

    size_t mask = set->cap - 1;
    size_t i = set->hash(data) & mask;
    size_t tomb = SIZE_MAX;

    for (;;) {
        if (set->state[i] == HS_EMPTY) {
            size_t dest = tomb != SIZE_MAX ? tomb : i;
            set->slots[dest] = (void *)data;
            set->state[dest] = HS_LIVE;
            if (tomb != SIZE_MAX) {
                set->tombs--;
            }
            set->len++;
            return 0;
        }
        if (set->state[i] == HS_TOMB && tomb == SIZE_MAX) {
            tomb = i;
        } else if (set->state[i] == HS_LIVE &&
                   set->match(set->slots[i], data) == 0) {
            return 1;
        }
        i = (i + 1) & mask;
    }
}

int lf_hashset_remove(LfHashSet *set, void **data) {
    if (set == NULL || data == NULL || set->match == NULL || set->hash == NULL ||
        set->cap == 0) {
        return -1;
    }

    size_t mask = set->cap - 1;
    size_t i = set->hash(*data) & mask;
    for (;;) {
        if (set->state[i] == HS_EMPTY) {
            return -1;
        }
        if (set->state[i] == HS_LIVE &&
            set->match(set->slots[i], *data) == 0) {
            *data = set->slots[i];
            set->slots[i] = NULL;
            set->state[i] = HS_TOMB;
            set->len--;
            set->tombs++;
            return 0;
        }
        i = (i + 1) & mask;
    }
}

int lf_hashset_is_member(const LfHashSet *set, const void *data) {
    if (set == NULL || set->match == NULL || set->hash == NULL ||
        set->cap == 0) {
        return 0;
    }

    size_t mask = set->cap - 1;
    size_t i = set->hash(data) & mask;
    for (;;) {
        if (set->state[i] == HS_EMPTY) {
            return 0;
        }
        if (set->state[i] == HS_LIVE &&
            set->match(set->slots[i], data) == 0) {
            return 1;
        }
        i = (i + 1) & mask;
    }
}
