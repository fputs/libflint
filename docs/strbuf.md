A growable owned string builder. Include `lfstrbuf.h`. Complements the borrowed
`LfStr` views in [[string]]. Length excludes the trailing NUL; views may append
embedded NUL bytes. Handles are caller-owned; the buffer owns its allocation.

## Structs

```c
typedef struct {
    char *data;
    size_t len, cap;
} LfStrBuf;
```

`cap` includes space for the terminator. An initialized buffer always has a
trailing NUL. A zeroed, detached, or destroyed handle has NULL data and zero
length/capacity and can be reused by reserve or append. Do not initialize a live
buffer. Allocation arithmetic is checked; allocation failure leaves existing
contents unchanged. Borrowed pointers may be invalidated by reserve or append.

## Functions

### lf_strbuf_init / lf_strbuf_reserve

Return 0 on success, -1 on invalid arguments, overflow, or allocation failure.
Init allocates an empty string; failed init leaves a destroyable handle. Reserve
ensures space for the requested total text length plus a terminator.

```c
int lf_strbuf_init(LfStrBuf *buf);
int lf_strbuf_reserve(LfStrBuf *buf, size_t length);

/* Usage */
LfStrBuf buf;
if (lf_strbuf_init(&buf) == 0) {
    if (lf_strbuf_append(&buf, "count: ") == 0 &&
        lf_strbuf_appendf(&buf, "%d", 42) == 0) {
        /* buf.data is "count: 42", buf.len is 9 */
    }
    lf_strbuf_destroy(&buf);
}
```

### lf_strbuf_append / lf_strbuf_append_view

Append a C string or counted view. Return 0 on success, -1 on error. The source
may be a valid view into the buffer itself, even when appending requires growth.
NULL C strings are invalid; a NULL view pointer is valid only with zero length.

```c
int lf_strbuf_append(LfStrBuf *buf, const char *text);
int lf_strbuf_append_view(LfStrBuf *buf, LfStr text);
```

### lf_strbuf_appendf / lf_strbuf_vappendf

Append using C99 `vsnprintf` formatting. Return 0 on success, -1 on allocation,
size, argument, or formatting error. Formatting uses a temporary allocation, so
format strings and string arguments may refer to the existing buffer. The
supplied `va_list` is not consumed. Follow printf argument/type rules; do not use
`%n` to modify buffer state or format inputs (formatting runs twice).

```c
int lf_strbuf_appendf(LfStrBuf *buf, const char *format, ...);
int lf_strbuf_vappendf(LfStrBuf *buf, const char *format, va_list args);
```

### lf_strbuf_clear

Set length to zero, retaining allocation. Accepts NULL.

```c
void lf_strbuf_clear(LfStrBuf *buf);
```

### lf_strbuf_detach

Transfer the NUL-terminated allocation to the caller, who must `free` it, and
zero the handle. Save `len` first if needed. Detaching a zeroed handle allocates
an empty string. Returns NULL on error and leaves the buffer unchanged.

```c
char *lf_strbuf_detach(LfStrBuf *buf);
```

### lf_strbuf_destroy

Free storage and zero the handle. Accepts NULL and repeated destruction.

```c
void lf_strbuf_destroy(LfStrBuf *buf);
```
