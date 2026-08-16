#ifndef LIBFLINT_NET_H
#define LIBFLINT_NET_H

#include <sys/socket.h>

/*
 * Bind helpers. There is no Server runloop, no thread-per-connection
 * echo, and no process-global SIGCHLD handler. IPv4 only.
 * host == NULL binds INADDR_ANY (AI_PASSIVE).
 */

int lf_listen_tcp(const char *host, const char *port);
int lf_bind_udp(const char *host, const char *port);
void lf_close(int fd);
void *get_in_addr(struct sockaddr *sa);

#endif /* LIBFLINT_NET_H */
