#include "lfparsing.h"

static int ses_score_sw(char c) {
    switch (c) {
        case 'e': case 'E': return 12;
        case 't': case 'T': return 11;
        case 'a': case 'A': return 10;
        case 'o': case 'O': return 9;
        case 'i': case 'I': return 8;
        case 'n': case 'N': return 7;
        case 's': case 'S': return 6;
        case 'h': case 'H': return 5;
        case 'r': case 'R': return 4;
        case 'd': case 'D': return 3;
        case 'l': case 'L': return 2;
        case 'u': case 'U': return 1;
        default: return 0;
    }
}

int simple_english_scoring(const char *s) {
    int score = 0;
    for (const char *c = s; *c != '\0'; ++c) {
        score += ses_score_sw(*c);
    }
    return score;
}
