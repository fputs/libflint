#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "lftest.h"
#include "lfnetwork.h"

#define NET_MSG "TEST SEND"

static int set_timeout(int fd) {
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv) != 0) {
        return -1;
    }
    return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
}

static int bound_port(int fd) {
    struct sockaddr_in addr;
    socklen_t len = sizeof addr;
    if (getsockname(fd, (struct sockaddr *)&addr, &len) != 0) {
        return -1;
    }
    return ntohs(addr.sin_port);
}

static int connect_tcp(const char *host, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    if (set_timeout(fd) != 0) {
        close(fd);
        return -1;
    }
    struct sockaddr_in a;
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, host, &a.sin_addr) != 1) {
        close(fd);
        return -1;
    }
    if (connect(fd, (struct sockaddr *)&a, sizeof a) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static void test_bad_args(void) {
    ASSERT_EQ(lf_listen_tcp("127.0.0.1", "notaport"), -1);
    ASSERT_EQ(lf_listen_tcp("127.0.0.1", NULL), -1);
    ASSERT_EQ(lf_bind_udp("127.0.0.1", "notaport"), -1);
    ASSERT_EQ(lf_bind_udp("127.0.0.1", NULL), -1);
    lf_close(-1);
}

static void test_get_in_addr(void) {
    struct sockaddr_in in;
    memset(&in, 0, sizeof in);
    in.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &in.sin_addr);
    ASSERT_EQ(get_in_addr((struct sockaddr *)&in), (void *)&in.sin_addr);
    ASSERT_NULL(get_in_addr(NULL));
}

static void test_tcp(void) {
    int srv = lf_listen_tcp("127.0.0.1", "0");
    ASSERT_TRUE(srv >= 0);
    ASSERT_EQ(set_timeout(srv), 0);
    int port = bound_port(srv);
    ASSERT_TRUE(port > 0);

    int cli = connect_tcp("127.0.0.1", port);
    ASSERT_TRUE(cli >= 0);

    int acc = accept(srv, NULL, NULL);
    ASSERT_TRUE(acc >= 0);
    ASSERT_EQ(set_timeout(acc), 0);

    size_t msg_len = strlen(NET_MSG);
    ASSERT_EQ((size_t)send(acc, NET_MSG, msg_len, 0), msg_len);

    char buf[32];
    memset(buf, 0, sizeof buf);
    ssize_t n = recv(cli, buf, sizeof buf - 1, 0);
    ASSERT_EQ((size_t)n, msg_len);
    buf[n] = '\0';
    ASSERT_STR_EQ(buf, NET_MSG);

    lf_close(acc);
    lf_close(cli);
    lf_close(srv);
}

static void test_udp(void) {
    int srv = lf_bind_udp("127.0.0.1", "0");
    ASSERT_TRUE(srv >= 0);
    ASSERT_EQ(set_timeout(srv), 0);
    int port = bound_port(srv);
    ASSERT_TRUE(port > 0);

    int cli = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_TRUE(cli >= 0);
    ASSERT_EQ(set_timeout(cli), 0);

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof dest);
    dest.sin_family = AF_INET;
    dest.sin_port = htons((uint16_t)port);
    ASSERT_EQ(inet_pton(AF_INET, "127.0.0.1", &dest.sin_addr), 1);

    size_t msg_len = strlen(NET_MSG);
    ASSERT_EQ((size_t)sendto(cli, NET_MSG, msg_len, 0,
                             (struct sockaddr *)&dest, sizeof dest),
              msg_len);

    char recv_buf[128];
    memset(recv_buf, 0, sizeof recv_buf);
    struct sockaddr_storage from;
    socklen_t from_len = sizeof from;
    ssize_t r = recvfrom(srv, recv_buf, sizeof recv_buf - 1, 0,
                         (struct sockaddr *)&from, &from_len);
    ASSERT_TRUE(r > 0);
    ASSERT_EQ((size_t)r, msg_len);
    recv_buf[r] = '\0';
    ASSERT_STR_EQ(recv_buf, NET_MSG);

    lf_close(cli);
    lf_close(srv);
}

int main(void) {
    test_bad_args();
    test_get_in_addr();
    test_tcp();
    test_udp();
    TEST_REPORT();
}
