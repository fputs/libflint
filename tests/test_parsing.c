#include <stdio.h>
#include "lftest.h"
#include "lfparsing.h"

int main() {
    char *nonsense = "8d82jI|dms~<>s2d";
    char *english = "This is an English sentence!";
    ASSERT_TRUE(simple_english_scoring(english) > simple_english_scoring(nonsense));

    /* NULL input */
    ASSERT_EQ(simple_english_scoring(NULL), -1);

    /* empty string */
    ASSERT_EQ(simple_english_scoring(""), 0);

    /* known absolute score: "the" = t(12) + h(5) + e(13) = 30 */
    ASSERT_EQ(simple_english_scoring("the"), 30);

    /* single space scores 7 */
    ASSERT_EQ(simple_english_scoring(" "), 7);

    /* unknown chars score 0 */
    ASSERT_EQ(simple_english_scoring("!!!"), 0);

    TEST_REPORT();
}
