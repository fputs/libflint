#ifndef LIBFLINT_PROCESS_H
#define LIBFLINT_PROCESS_H

#include <stddef.h>

#define LF_CAPTURE_SYSTEM_BUFSIZE 1024

/*
 * popen() the command through /bin/sh with no sanitizing.
 * Never pass untrusted input. Not a file-slurp helper.
 */
char *lf_capture_system(const char *cmd, size_t buf_sz);

#endif /* LIBFLINT_PROCESS_H */
