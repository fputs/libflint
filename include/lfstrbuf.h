#ifndef LIBFLINT_H_STRBUF
#define LIBFLINT_H_STRBUF
#include <stdarg.h>
#include "lfstring.h"

typedef struct {
    char *data;
    size_t len, cap; /* cap includes the terminator */
} LfStrBuf;

int lf_strbuf_init(LfStrBuf *buf);
/* Reserve total text length, excluding the trailing NUL. */
int lf_strbuf_reserve(LfStrBuf *buf, size_t length);
int lf_strbuf_append(LfStrBuf *buf, const char *text);
int lf_strbuf_append_view(LfStrBuf *buf, LfStr text);
#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 2, 3)))
#endif
int lf_strbuf_appendf(LfStrBuf *buf, const char *format, ...);
#if defined(__GNUC__) || defined(__clang__)
__attribute__((format(printf, 2, 0)))
#endif
int lf_strbuf_vappendf(LfStrBuf *buf, const char *format, va_list args);
void lf_strbuf_clear(LfStrBuf *buf);
/* Transfers allocation; returns NULL on error. Reset handle is reusable. */
char *lf_strbuf_detach(LfStrBuf *buf);
void lf_strbuf_destroy(LfStrBuf *buf);
#endif
