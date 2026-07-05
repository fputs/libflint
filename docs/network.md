This module provides a generic `Server` type that abstracts away the setup and teardown of a socket
## Enums

### ServerType

Types of servers. Currently supports TCP and UDP, will eventually add UNIX sockets.

```c
typedef enum ServerType {
    SERVERTYPE_TCP,
    SERVERTYPE_UDP
} ServerType;
```

## Structs

### Server

Server is a generic abstraction over sockets. The type of the server is defined by `server_type`.

```c
typedef struct Server {
    ServerType server_type;
    int fd;
    int port;
    void (*handler)(struct Server *s);
} Server;
```

## Functions

### new_server

Create a `Server*`. User is responsible for freeing the memory.

```c
Server *new_server(ServerType type, const char *port, void(handler)(Server *s));
```

### delete_server

Closes the server's socket and frees the memory allocated for `Server*`. Safe to call with `NULL`.

```c
void delete_server(Server *s);
```

### serve

Starts up the server. `backlog_size` is the size of the backlog buffer for the underlying socket (ignored for UDP). Use the macro `DEFAULT_BACKLOG` or pass `0` to use a reasonable default. Returns `0` on success, `-1` on error.

```c
int serve(Server *s, int backlog_size);
```

### get_in_addr

Convenience method to get an IP address from a `struct sockaddr` of either IPV4 or IPV6.

```c
void *get_in_addr(struct sockaddr *sa);

/* Usage */
struct sockaddr_storage client_addr;
socklen_t client_addr_sz = sizeof(client_addr);
char buf[33];

if ((new_fd = accept(s->fd, (struct sockaddr *)&client_addr, &client_addr_sz)) == -1) {
    /* error handling */
}
inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), buf, 32);
printf("Received connection from %s\n", buf);
```

### handler_tcp_echo

An example handler for a multithreaded tcp echo server. Spawns one detached thread per connection with no connection limit or timeout — intended as a starting point, not for production use. Write your own handler for real applications.

```c
void handler_tcp_echo(Server *s);

/* Usage */
#include "lfnetwork.h"

int main(int argc, char **argv) {
    Server *server = new_server(SERVERTYPE_TCP, "80", handler_tcp_echo);
    serve(server, DEFAULT_BACKLOG);
    delete_server(server);
}
```

## Macros

### DEFAULT_BACKLOG

A default size for the socket's backlog buffer. `5` is a standard default size, providing some backlog but not enough to make huge buffers for each socket

```c
#define DEFAULT_BACKLOG 5
```