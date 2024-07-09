#ifndef LIBFLINT_NET_H
#define LIBFLINT_NET_H

#include <netdb.h>

typedef enum ServerType {
    SERVERTYPE_TCP,
    SERVERTYPE_UDP
} ServerType;

typedef struct Server {
    ServerType server_type;
    int fd;
    int port;
    void (*handler)(struct Server *s);
} Server;

#define DEFAULT_BACKLOG 5

Server *new_server(ServerType type, const char *port, void(handler)(Server *s));
void delete_server(Server *s);
int serve(Server *s, int backlog_size);
void *get_in_addr(struct sockaddr *sa);

// Example handlers
void handler_tcp_echo(Server *s);

#endif //LIBFLINT_NET_H
