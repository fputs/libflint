#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "lfnetwork.h"

#define LF_LISTEN_BACKLOG 5

void *get_in_addr(struct sockaddr *sa)
{
    if (sa == NULL) {
        return NULL;
    }
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    if (sa->sa_family == AF_INET6) {
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
    return NULL;
}

static int lf_bind_sock(const char *host, const char *port, int socktype, int do_listen) {
    if (port == NULL) {
        return -1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = socktype;
    if (host == NULL) {
        hints.ai_flags = AI_PASSIVE;
    }

    struct addrinfo *addr = NULL;
    if (getaddrinfo(host, port, &hints, &addr) != 0) {
        return -1;
    }

    int fd = -1;
    for (struct addrinfo *p = addr; p != NULL; p = p->ai_next) {
        fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (fd == -1) {
            continue;
        }

        int yes = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

        if (bind(fd, p->ai_addr, p->ai_addrlen) != 0) {
            close(fd);
            fd = -1;
            continue;
        }
        if (do_listen && listen(fd, LF_LISTEN_BACKLOG) != 0) {
            close(fd);
            fd = -1;
            continue;
        }
        break;
    }

    freeaddrinfo(addr);
    return fd;
}

int lf_listen_tcp(const char *host, const char *port) {
    return lf_bind_sock(host, port, SOCK_STREAM, 1);
}

int lf_bind_udp(const char *host, const char *port) {
    return lf_bind_sock(host, port, SOCK_DGRAM, 0);
}

void lf_close(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}
