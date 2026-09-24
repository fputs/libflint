IPv4 socket helpers for listening over TCP and binding over UDP. You are responsible for accepting connections, sending and receiving data, and closing sockets.

## Usage

Create a listening socket with `lf_listen_tcp()`. Pass `NULL` as the host to bind to all local IPv4 interfaces. Port `"0"` lets the kernel choose a port; use `getsockname()` to read it back.

```c
int fd = lf_listen_tcp("127.0.0.1", "8080");
if (fd >= 0) {
    int client = accept(fd, NULL, NULL);
    if (client >= 0) {
        /* Exchange data with the client. */
        lf_close(client);
    }
    lf_close(fd);
}
```

`accept()` blocks until a connection arrives or an error occurs. The returned sockets are yours to close.

## Functions

### lf_listen_tcp

Creates an IPv4 TCP socket, binds it, and starts listening. Returns the socket descriptor, or `-1` on error. `port` must be non-NULL.

```c
int lf_listen_tcp(const char *host, const char *port);
```

### lf_bind_udp

Creates an IPv4 UDP socket and binds it. Returns the socket descriptor, or `-1` on error. The host and port rules are the same as `lf_listen_tcp()`.

```c
int lf_bind_udp(const char *host, const char *port);
```

### lf_close

Closes `fd` if it is nonnegative. A negative descriptor does nothing.

```c
void lf_close(int fd);
```

### get_in_addr

Returns a pointer to the IPv4 or IPv6 address inside `sa`. The pointer borrows storage from `sa`. Returns `NULL` if `sa` is NULL or has an unsupported address family. This helper accepts IPv6 addresses even though the bind helpers only create IPv4 sockets.

```c
void *get_in_addr(struct sockaddr *sa);
```
