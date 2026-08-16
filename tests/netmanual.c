#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#include "lfnetwork.h"

int main(void) {
    int fd = lf_listen_tcp("127.0.0.1", "18632");
    if (fd < 0) {
        fprintf(stderr, "lf_listen_tcp failed\n");
        return 1;
    }

    for (;;) {
        int client = accept(fd, NULL, NULL);
        if (client < 0) {
            continue;
        }
        char buf[256];
        ssize_t n;
        while ((n = recv(client, buf, sizeof buf, 0)) > 0) {
            if (send(client, buf, (size_t)n, 0) < 0) {
                break;
            }
        }
        close(client);
    }
}
