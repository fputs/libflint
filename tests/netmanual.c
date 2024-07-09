#include "lfnetwork.h"

int main(int argc, char **argv) {
    Server *server = new_server(SERVERTYPE_TCP, "18632", handler_tcp_echo);
    serve(server, DEFAULT_BACKLOG);
    delete_server(server);
}
