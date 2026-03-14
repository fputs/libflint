#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "lftest.h"
#include "lfnetwork.h"
#include "lfinput.h"

#define NET_MSG "TEST SEND"

static void tcp_test_handler(Server *s) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_sz = sizeof(client_addr);
    int new_fd = accept(s->fd, (struct sockaddr *) &client_addr, &client_addr_sz);
    ASSERT_NEQ(new_fd, -1);
    ASSERT_NEQ((int)send(new_fd, NET_MSG, 10, 0), -1);
    close(new_fd);
}

static void *tcp_server_thread(void *vargp) {
    (void)vargp;
    Server *server = new_server(SERVERTYPE_TCP, "18632", tcp_test_handler);
    serve(server, DEFAULT_BACKLOG);
    delete_server(server);
    return NULL;
}

static void udp_test_handler(Server *s) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_sz = sizeof(client_addr);
    char recv_buf[128];

    int r = (int) recvfrom(s->fd, recv_buf, 128, 0, (struct sockaddr *) &client_addr, &client_addr_sz);
    ASSERT_TRUE(r > 0);
    ASSERT_STR_EQ(recv_buf, NET_MSG);
}

static void *udp_server_thread(void *vargp) {
    (void)vargp;
    Server *server = new_server(SERVERTYPE_UDP, "18633", udp_test_handler);
    serve(server, DEFAULT_BACKLOG);
    delete_server(server);
    return NULL;
}

static void dummy_handler(Server *s) {
    (void)s;
}

int main() {
    /* Test new_server with bad port returns NULL */
    Server *bad = new_server(SERVERTYPE_TCP, "notaport", dummy_handler);
    ASSERT_NULL(bad);

    /* Test new_server creates valid TCP server and delete_server cleans up */
    Server *tcp = new_server(SERVERTYPE_TCP, "18640", dummy_handler);
    ASSERT_NOT_NULL(tcp);
    ASSERT_EQ(tcp->server_type, SERVERTYPE_TCP);
    ASSERT_EQ(tcp->port, 18640);
    ASSERT_TRUE(tcp->fd >= 0);
    delete_server(tcp);

    /* Test new_server creates valid UDP server and delete_server cleans up */
    Server *udp = new_server(SERVERTYPE_UDP, "18641", dummy_handler);
    ASSERT_NOT_NULL(udp);
    ASSERT_EQ(udp->server_type, SERVERTYPE_UDP);
    ASSERT_EQ(udp->port, 18641);
    ASSERT_TRUE(udp->fd >= 0);
    delete_server(udp);

    /* Test delete_server with NULL doesn't crash */
    delete_server(NULL);

    /* TCP echo test */
    pthread_t srv_tid;
    pthread_create(&srv_tid, NULL, tcp_server_thread, NULL);

    sleep(1);
    char *s = inp_capture_system("echo hello | nc localhost 18632", 0);
    ASSERT_STR_EQ(s, NET_MSG);
    free(s);

    pthread_join(srv_tid, NULL);

    /* UDP test */
    pthread_create(&srv_tid, NULL, udp_server_thread, NULL);
    sleep(1);
    system("echo hello | nc localhost 18633");

    pthread_join(srv_tid, NULL);

    TEST_REPORT();
}
