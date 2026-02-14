#include <stdio.h>
#include "lftest.h"
#include "lfparsing.h"

int main() {
    char *nonsense = "8d82jI|dms~<>s2d";
    char *english = "This is an English sentence!";
    ASSERT_TRUE(simple_english_scoring(english) > simple_english_scoring(nonsense));

    TEST_REPORT();
}
