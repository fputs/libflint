#ifndef LIBFLINT_COMPAT_H
#define LIBFLINT_COMPAT_H

#include <stddef.h>

/*
 * Overflow-checked realloc for nmemb * size bytes.
 * Same contract as POSIX reallocarray, under a prefixed name so it does
 * not clash with libc, libbsd, or newer SDKs.
 */
void *lf_reallocarray(void *ptr, size_t nmemb, size_t size);

#endif /* LIBFLINT_COMPAT_H */
