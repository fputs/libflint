IPv4 bind helpers. There is no `Server` type, no runloop, and no thread-per-connection echo in the library. `lf_listen_tcp` returns a listening TCP fd; `lf_bind_udp` returns a bound UDP fd. The caller owns accept, recv, send, and shutdown.

`host == NULL` binds `INADDR_ANY`. Port `"0"` lets the kernel pick an ephemeral port — use `getsockname` to read it back.

## Functions

### lf_listen_tcp

Creates an IPv4 TCP socket, binds it, and `listen`s. Returns the fd, or `-1` on error.

```c
int lf_listen_tcp(const char *host, const char *port);

/* Usage */
int fd = lf_listen_tcp("127.0.0.1", "8080");
int client = accept(fd, NULL, NULL);
lf_close(client);
lf_close(fd);
```

### lf_bind_udp

Creates an IPv4 UDP socket and binds it. Returns the fd, or `-1` on error.

```c
int lf_bind_udp(const char *host, const char *port);
```

### lf_close

Closes `fd` if it is `>= 0`. Safe with `-1`.

```c
void lf_close(int fd);
```

### get_in_addr

Returns a pointer to the IPv4 or IPv6 address inside a `struct sockaddr`. Returns NULL if `sa` is NULL.

```c
void *get_in_addr(struct sockaddr *sa);

/* Usage */
struct sockaddr_storage client_addr;
socklen_t client_addr_sz = sizeof(client_addr);
char buf[INET6_ADDRSTRLEN];
int new_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_sz);
inet_ntop(client_addr.ss_family,
          get_in_addr((struct sockaddr *)&client_addr),
          buf, sizeof buf);
```
