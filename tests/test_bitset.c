#include "lfbitset.h"
#include "lftest.h"
int main(void) {
    const size_t sizes[] = {0, 1, 63, 64, 65, 127, 128, 129};
    for (size_t n = 0; n < sizeof(sizes) / sizeof(*sizes); n++) {
        LfBitSet a, b, other;
        size_t size = sizes[n];
        ASSERT_EQ(lf_bitset_init(&a, size), 0);
        ASSERT_EQ(lf_bitset_init(&b, size), 0);
        ASSERT_EQ(lf_bitset_init(&other, size + 1), 0);
        ASSERT_EQ(lf_bitset_count(&a), 0);
        int value = 42;
        ASSERT_EQ(lf_bitset_test(&a, size, &value), -1); ASSERT_EQ(value, 42);
        ASSERT_EQ(lf_bitset_set(&a, SIZE_MAX), -1);
        ASSERT_EQ(lf_bitset_clear(&a, size), -1);
        ASSERT_EQ(lf_bitset_toggle(&a, size), -1);
        for (size_t i = 0; i < size; i++) {
            ASSERT_EQ(lf_bitset_set(&a, i), 0);
            ASSERT_EQ(lf_bitset_test(&a, i, &value), 0); ASSERT_EQ(value, 1);
            if (i % 2 == 0) ASSERT_EQ(lf_bitset_set(&b, i), 0);
        }
        ASSERT_EQ(lf_bitset_count(&a), size);
        ASSERT_EQ(lf_bitset_union(&a, &other), -1);
        ASSERT_EQ(lf_bitset_intersection(&a, &other), -1);
        ASSERT_EQ(lf_bitset_count(&a), size);
        ASSERT_EQ(lf_bitset_intersection(&a, &b), 0);
        ASSERT_EQ(lf_bitset_count(&a), (size + 1) / 2);
        for (size_t i = 0; i < size; i++) ASSERT_EQ(lf_bitset_toggle(&a, i), 0);
        ASSERT_EQ(lf_bitset_count(&a), size / 2);
        ASSERT_EQ(lf_bitset_union(&a, &b), 0); ASSERT_EQ(lf_bitset_count(&a), size);
        ASSERT_EQ(lf_bitset_intersection(&a, &a), 0);
        if (size % 64) ASSERT_EQ(a.words[a.nwords - 1] >> (size % 64), 0);
        for (size_t i = 0; i < size; i++) ASSERT_EQ(lf_bitset_clear(&a, i), 0);
        ASSERT_EQ(lf_bitset_count(&a), 0);
        lf_bitset_destroy(&a); lf_bitset_destroy(&a);
        lf_bitset_destroy(&b); lf_bitset_destroy(&other);
    }
    TEST_REPORT();
}
