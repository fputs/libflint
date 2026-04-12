#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "lfnetwork.h"

static void sighandler(int s) {
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Server *new_server(ServerType type, const char *port, void(handler)(Server *s)) {
    Server *s = (Server *)malloc(sizeof(Server));
    if (s == NULL) {
        return NULL;
    }
    s->server_type = type;
    s->handler = handler;

    int socktype = 0;
    switch (type) {
    case SERVERTYPE_TCP:
        socktype = SOCK_STREAM;
        break;
    case SERVERTYPE_UDP:
        socktype = SOCK_DGRAM;
        break;
    }

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = socktype;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *addr = NULL;
    if (getaddrinfo(NULL, port, &hints, &addr) != 0) {
        free(s);
        return NULL;
    }
    s->port = (int)strtol(port, NULL, 10);

    struct addrinfo *p;
    for (p = addr; p != NULL; p = p->ai_next) {
        s->fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s->fd == -1) {
            continue;
        }

        int yes = 1;
        setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        if (bind(s->fd, p->ai_addr, p->ai_addrlen) != 0) {
            close(s->fd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to bind\n");
        freeaddrinfo(addr);
        free(s);
        return NULL;
    }

    freeaddrinfo(addr);
    return s;
}

void delete_server(Server *s) {
    if (s == NULL) return;
    close(s->fd);
    free(s);
}

int serve(Server *s, int backlog_size) {
    if (backlog_size == 0) {
        backlog_size = DEFAULT_BACKLOG;
    }

    if (s->server_type != SERVERTYPE_UDP) {
        if (listen(s->fd, backlog_size) != 0) {
            return -1;
        }
    }

    // Linux doesn't handle SA_RESTART properly, and I don't know about Windows (nor do I care)
    // This is just for macOS and BSDs
    #if !defined(__linux__) && !defined(_WIN32)
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sighandler;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "Failed to set sigaction\n");
        return -1;
    }
    #endif

    s->handler(s);

    return 0;
}

/* recv_buf is not NUL-terminated — safe here (send uses byte count), but
 * do not treat it as a C string without adding a terminator first. */
static void *tcp_echo_thread(void *vargp) {
    int fd = (int)(intptr_t)vargp;
    while (1) {
        char recv_buf[256];

        int r = (int)recv(fd, recv_buf, 256, 0);
        if (r < 1) {
            if (r == -1) {
                fprintf(stderr, "Failed to recv. Errno: %d\n", errno);
            }
            close(fd);
            break;
        }

        if (send(fd, recv_buf, r, 0) == -1) {
            fprintf(stderr, "Failed to send echo. Errno: %d\n", errno);
            close(fd);
            break;
        }
    }
    return NULL;
}

/*
 * Basic example handler — spawns one detached thread per connection with no
 * connection limit or timeout. Not suitable for production use.
 */
void handler_tcp_echo(Server *s) {
    while (1) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_sz = sizeof(client_addr);
        int new_fd = accept(s->fd, (struct sockaddr *)&client_addr, &client_addr_sz);
        if (new_fd == -1) {
            fprintf(stderr, "failed to accept. Errno: %d\n", errno);
            continue;
        }

        pthread_t srv_tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&srv_tid, &attr, tcp_echo_thread, (void *)(intptr_t)new_fd);
        pthread_attr_destroy(&attr);
    }
}
