Compatibility page. This module is not cryptography.

- Hex and Base64 live in [[codec]] (`lfcodec.h`).
- Repeating-key XOR, Hamming distance, and English scoring live in [[puzzle]] (`lfpuzzle.h`).

`lfcrypto.h` includes both headers so existing `#include "lfcrypto.h"` still compiles.
