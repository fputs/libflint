# lfhashmap

Include `lfhashmap.h`. `LfHashMap` is a caller-owned, open-addressed map of
pointer keys to pointer values. Initialize with an initial capacity (zero chooses
eight), equality callback (zero means equal), hash callback, and independent key
and value destruction callbacks. Equal keys must hash equally; keys must remain
stable while stored. NULL keys are allowed if both callbacks support them.

`lf_hashmap_insert` returns 0 on insertion, 1 on duplicate, and -1 on error.
Ownership passes only on success. `lf_hashmap_replace` replaces an existing pair,
returning 1 if absent. It destroys each displaced pointer except when identical
to the corresponding new pointer. Keys and values must have independent lifetimes
when using owning callbacks: do not share owned allocations across entries or
between key and value roles. Callbacks must not mutate the map.

`lf_hashmap_find(map, key, &value)` returns 0 for a hit, 1 for a miss, or -1 for
invalid arguments, so a stored NULL is distinct from a miss. `lf_hashmap_remove`
takes separate, required key and value outputs and transfers both pointers without
calling destructors. Outputs remain unchanged on failure and must not alias map
storage. `lf_hashmap_clear` invokes callbacks (including for NULL), retaining
capacity; destroy also frees storage and zeros the handle. NULL callbacks mean
borrowed pointers. NULL handles are accepted by clear and destroy.

Allocation arithmetic is checked. Failed insertion leaves contents and ownership
unchanged. Initialization failure leaves a destroyable handle; do not initialize
an already live map. Expected lookup/insertion/removal time is O(1); rehashing and
clear/destroy are O(capacity). No synchronization is provided.
