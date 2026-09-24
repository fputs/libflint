A compatibility header for the codec and puzzle helpers.

## Usage

Include `lfcrypto.h` to get both sets of functions. For new code, include the header for the functions you use:

```c
#include "lfcodec.h"
#include "lfpuzzle.h"
```

See [[codec]] for Hex and Base64, and [[puzzle]] for repeating-key XOR, Hamming distance, and English scoring. These helpers are not cryptographic primitives.
