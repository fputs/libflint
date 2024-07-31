#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "lflinkedlist.h"
#include "lfnetwork.h"
#include "lfset.h"
#include "lfstack.h"
#include "lfbinarytree.h"
#include "lfvector.h"
#include "lfmath.h"
#include "lfstring.h"
#include "lfcrypto.h"
#include "lfparsing.h"
#include "lfinput.h"
#include "lfmemory.h"

#if defined(__APPLE__) || defined(__MACH__)
#include "lfmacos.h"
#endif /* defined(__APPLE__) || defined(__MACH__) */

void print_ll(List *list) {
    LL_ITER(list) {
        printf(" %d", *((int *) node->data));
    }
    printf("\n");
}

void test_ll() {
    printf("\n--- LIST TEST ---\n");
    List *list = malloc(sizeof(List));
    ll_init(list, NULL);

    int i = 1;
    int j = 2;
    int k = 4;

    ll_ins_next(list, list->head, (void *) &i);
    ll_ins_next(list, list->tail, (void *) &j);
    ll_ins_next(list, list->tail, (void *) &k);

    printf("List: ");
    print_ll(list);

    void *data;
    ll_remove_next(list, list->head, &data);

    printf("List: ");
    print_ll(list);
    assert(*(int*)data == 2);
    printf("Removed: %d\n", *((int *) data));

    ll_destroy(list);
    free(list);
}

int int_match(const void *a, const void *b) {
    return *((int *) a) == *((int *) b);
}

void test_set() {
    printf("\n--- SET TEST ---\n");
    Set *set = malloc(sizeof(Set));
    set_init(set, int_match, NULL);

    int i = 1;
    int j = 2;
    int k = 2;

    set_insert(set, &i);
    set_insert(set, &j);
    set_insert(set, &k);

    int i2 = 1;
    int j2 = 4;

    Set *set2 = malloc(sizeof(Set));
    set_init(set2, int_match, NULL);

    set_insert(set2, &i2);
    set_insert(set2, &j2);

    printf("Set 1:");
    print_ll(set);

    printf("Set 2:");
    print_ll(set2);
    printf("\n");

    Set *set_u = malloc(sizeof(Set));
    Set *set_i = malloc(sizeof(Set));
    Set *set_d = malloc(sizeof(Set));

    set_union(set_u, set, set2);
    printf("Union:");
    print_ll(set_u);

    set_difference(set_d, set, set2);
    printf("Difference:");
    print_ll(set_d);

    set_intersection(set_i, set, set2);
    printf("Intersection:");
    print_ll(set_i);

    set_destroy(set);
    set_destroy(set2);
    set_destroy(set_u);
    set_destroy(set_i);
    set_destroy(set_d);
    free(set);
    free(set2);
    free(set_u);
    free(set_i);
    free(set_d);
}

void test_stack() {
    printf("\n--- STACK TEST ---\n");
    Stack *stack = malloc(sizeof(Stack));
    stack_init(stack, NULL);

    int a = 1, b = 2;
    stack_push(stack, &a);
    stack_push(stack, &b);
    printf("Stack size: %lu\n", stack->size);

    int *p = NULL;
    stack_pop(stack, (void **) &p);
    printf("b = %d\n", *p);

    stack_pop(stack, (void **) &p);
    printf("a = %d\n", *p);
    printf("Stack size: %lu\n", stack->size);

    stack_destroy(stack);
    free(stack);
    stack = NULL;
}

void test_bintree() {
    printf("\n--- BINARY TREE TEST ---\n");
    BinTree *tree = malloc(sizeof(BinTree));
    bintree_init(tree, NULL);

    int root = 0;
    int l1 = 1;
    int l2 = 2;
    int r1 = 12;
    int r2 = 200;

    bintree_ins_left(tree, NULL, &root);
    bintree_ins_left(tree, tree->root, &l1);
    bintree_ins_left(tree, tree->root->left, &l2);
    bintree_ins_right(tree, tree->root->left, &r2);
    bintree_ins_right(tree, tree->root, &r1);
    bintree_ins_right(tree, tree->root->right, &r2);
    bintree_ins_left(tree, tree->root->right, &l1);

    bintree_debug_print(tree);

    printf("Changing r2\n");
    r2 = 100;
    bintree_debug_print(tree);

    bintree_destroy(tree);
    free(tree);
    tree = NULL;
}

void test_math() {
    printf("\n--- MATH TEST ---\n");
    int i = 1, j = 2;
    assert(max_int(i, j) == j);
    printf("Between %d and %d, %d is larger\n", i, j, max_int(i, j));
    printf("Between %d and %d, %d is smaller\n", i, j, min_int(i, j));

    char *s = "10101101";
    printf("Binary: %s\n", s);
    printf("Decimal: %d\n", binstr_to_int(s));

    char *s2 = "1010_1101";
    printf("Binary: %s\n", s2);
    printf("Decimal: %d\n", binstr_to_int(s2));

    i = 10;
    i = clamp_int(i, 2, 5);
    assert(i == 5);

    printf("\nGenerate line from 0,0 to 2,5\n");
    size_t sz = 0;
    Point *line = bresenham(0, 0, 2, 5, &sz);
    for (size_t idx = 0; idx < sz; idx++) {
        printf("%d,%d ", line[idx].x, line[idx].y);
    }
    printf("\n");
    free(line);
}

void print_vector(Vector *vec) {
    for (size_t i = 0; i < vec->length; ++i) {
        int t = *(int *) vec_at(vec, i);
        printf("%d ", t);
    }
    printf("\n");
}

void test_vector() {
    printf("\n--- VECTOR TEST ---\n");
    Vector *v = malloc(sizeof(Vector));
    vec_init(v, NULL);

    int e0 = 0;
    int e1 = 1;
    int e2 = 2;
    int e3 = 3;
    int e4 = 4;

    size_t idx = 0;
    vec_push(v, &e0, &idx);
    assert(idx == 0);
    assert(v->length == 1);
    int *t = (int *) vec_at(v, 0);
    assert(*t == 0);

    vec_push(v, &e1, NULL);
    vec_push(v, &e2, NULL);
    assert(v->length == 3);

    // test access outside bounds
    t = (int *) vec_safe_at(v, 3);
    assert(t == NULL);

    printf("Before insert: ");
    print_vector(v);
    vec_push(v, &e4, NULL);
    vec_insert(v, &e3, 3);
    printf("After insert:  ");
    print_vector(v);

    t = (int *) vec_at(v, 3);
    assert(*t == e3);
    t = (int *) vec_at(v, 4);
    assert(*t == e4);

    const int *min = vec_min(v, vec_cmp_int);
    const int *max = vec_max(v, vec_cmp_int);
    printf("min: %d\n", *min);
    printf("max: %d\n", *max);
    assert(*min == e0);
    assert(*max == e4);

    t = (int *) vec_remove(v, 1);
    assert(t != NULL);
    assert(*t == 1);
    printf("After removal: ");
    print_vector(v);

    t = (int *) vec_remove(v, 10);
    assert(t == NULL);

    printf("\ncap before shrink: %zu\n", vec_cap(v));
    vec_shrink(v);
    assert(vec_len(v) == vec_cap(v));
    printf("cap after shrink: %zu\n", vec_cap(v));

    vec_destroy(v);
    free(v);
}

void test_string() {
    printf("\n--- STRING TEST ---\n");
    const char *haystack =
            "Test one two one and also maybe two but not Gabe's least favorite number, which is not one.";
    const char *needles[] = {
        "one",
        "two",
        "Gabe"
    };

    size_t sub_sz = 0;
    size_t *subs = NULL;
    find_substrings(haystack, needles[0], &sub_sz, &subs);

    assert(sub_sz == 3);
    assert(subs[0] == 5);
    assert(subs[1] == 13);
    assert(subs[2] == 87);

    char *s = substr(haystack, subs[0], strlen(needles[0]));
    assert(strcmp(s, needles[0]) == 0);

    free(s);
    free(subs);
    subs = NULL;

    find_substrings(haystack, needles[1], &sub_sz, &subs);
    assert(sub_sz == 2);
    assert(subs[0] == 9);

    free(subs);
    subs = NULL;

    find_substrings("test one two", "nope", &sub_sz, &subs);
    assert(sub_sz == 0);
    assert(subs == NULL);
    free(subs);
    subs = NULL;

    find_substrings("123", "nopes", &sub_sz, &subs);
    assert(sub_sz == 0);
    assert(subs == NULL);
    free(subs);
    subs = NULL;

    printf("Passes all string tests\n");
}

void test_crypto() {
    printf("\n--- CRYPTO TEST ---\n");

    char *in = "BUTT";
    unsigned char *s = b64_encode(in, strlen(in));
    assert(strcmp(s, "QlVUVA==") == 0);
    free(s);

    char *in2 = "a longer base64 test, apparently";
    s = b64_encode(in2, strlen(in2));
    assert(strcmp(s, "YSBsb25nZXIgYmFzZTY0IHRlc3QsIGFwcGFyZW50bHk=") == 0);
    free(s);

    char *out2 = "YSBsb25nZXIgYmFzZTY0IHRlc3QsIGFwcGFyZW50bHk=";
    size_t s_sz = 0;
    s = (char *) b64_decode(out2, strlen(out2), &s_sz);
    assert(strcmp(s, "a longer base64 test, apparently") == 0);
    assert(strlen(s) == s_sz);
    free(s);

    s = hex_decode("DEADBEEF", &s_sz);
    unsigned char h[4] = {
        0xDE, 0xAD, 0xBE, 0xEF
    };
    for (size_t i = 0; i < 4; ++i) {
        assert(s[i] == h[i]);
    }
    free(s);

    // Odd number of characters
    s = hex_decode("f00f5", &s_sz);
    unsigned char h2[4] = {
        0x0F, 0x00, 0xF5
    };
    for (size_t i = 0; i < 3; ++i) {
        assert(s[i] == h2[i]);
    }
    free(s);

    // leading 0x
    s = hex_decode("0xf00f5", &s_sz);
    for (size_t i = 0; i < 3; ++i) {
        assert(s[i] == h2[i]);
    }
    free(s);

    s = hex_encode(h, 4);
    assert(strcmp(s, "deadbeef") == 0);
    free(s);

    // "Sup?"
    unsigned char hexsup[4] = {
        0x53, 0x75, 0x70, 0x3F
    };
    s = hex_to_str(hexsup, 4);
    assert(strcmp(s, "Sup?") == 0);
    free(s);

    s = repeating_key_xor_s("TEST", "HI");
    char *enc = hex_encode(s, 4);
    assert(strcmp(enc, "1c0c1b1d") == 0);
    free(enc);
    free(s);

    unsigned char ua[2] = {0x2, 0xF};
    unsigned char ub[2] = {0x4, 0xE};
    unsigned int hamming = hamming_distance(ua, ub, 2);
    assert(hamming == 3);

    hamming = hamming_distance_s("this is a test", "wokka wokka!!!");
    assert(hamming == 37);

    printf("Passes all crypto tests\n");
}

void test_parsing() {
    printf("\n--- PARSING TEST ---\n");

    char *nonsense = "8d82jI|dms~<>s2d";
    char *english = "This is an English sentence!";
    assert(simple_english_scoring(english) > simple_english_scoring(nonsense));

    printf("Passes all parsing tests\n");
}

#define NET_MSG "TEST SEND"

void tcp_test_handler(Server *s) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_sz = sizeof(client_addr);
    int new_fd = accept(s->fd, (struct sockaddr *) &client_addr, &client_addr_sz);
    assert(new_fd != -1);
    assert(send(new_fd, NET_MSG, 10, 0) != -1);
    close(new_fd);
}

void *tcp_server_thread(void *vargp) {
    Server *server = new_server(SERVERTYPE_TCP, "18632", tcp_test_handler);
    serve(server, DEFAULT_BACKLOG);
    delete_server(server);
}

void udp_test_handler(Server *s) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_sz = sizeof(client_addr);
    char recv_buf[128];

    int r = (int) recvfrom(s->fd, recv_buf, 128, 0, (struct sockaddr *) &client_addr, &client_addr_sz);
    assert(r > 0);
    assert(strcmp(recv_buf, NET_MSG) == 0);
}

void *udp_server_thread(void *vargp) {
    Server *server = new_server(SERVERTYPE_UDP, "18633", udp_test_handler);
    serve(server, DEFAULT_BACKLOG);
    delete_server(server);
}

void test_network() {
    printf("\n--- NETWORK TEST ---\n");
    pthread_t srv_tid;
    pthread_create(&srv_tid, NULL, tcp_server_thread, NULL);

    sleep(1);
    const char *s = capture_system("echo hello | nc localhost 18632", 0);
    assert(strcmp(s, NET_MSG) == 0);
    free((char *) s);

    pthread_join(srv_tid, NULL);
    printf("Passed TCP test\n");

    pthread_create(&srv_tid, NULL, udp_server_thread, NULL);
    sleep(1);
    system("echo hello | nc localhost 18633");

    pthread_join(srv_tid, NULL);
    printf("Passed UDP test\n");
}

#if defined(__APPLE__) || defined(__MACH__)
void test_macos() {
    printf("\n--- macOS TEST ---\n");

    pid_t pid = getpid();
    ProcessData *pd = new_ProcessData();
    for (int i = 0; i < 4; i++) {
        update_process(pid, pd);
        printf("CPU: %.2f\n", pd->percent_cpu);
        sleep(1);
    }
    free(pd);
}
#endif

void test_memory() {
    printf("\n--- MEMORY TEST ---\n");
    ArenaAllocator *arena = malloc(sizeof(ArenaAllocator));
    arena_init(arena, 1024);

    int *i1 = arena_malloc(arena, sizeof(int));
    int *i2 = arena_malloc(arena, sizeof(int));

    *i1 = 1;
    *i2 = 2;

    assert(i1 < i2);
    assert(*i1 < *i2);

    long *l = arena_resize(arena, i1, sizeof(int), sizeof(long));
    assert(*l == 1);

    unsigned char *char_test = arena_resize(arena, i2, sizeof(int), sizeof(unsigned char));
    assert(*char_test == 2);

    arena_free(arena);
    arena = NULL;

    PoolAllocator *pool = malloc(sizeof(PoolAllocator));
    pool_init(pool, 64, 16, LF_DEFAULT_ALIGNMENT);
    void *a = pool_alloc(pool);
    void *b = pool_alloc(pool);
    void *c = pool_alloc(pool);
    void *d = pool_alloc(pool);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    assert(d != NULL);

    assert(pool_count_available(pool) == 0);
    pool_free(pool, d);
    d = NULL;
    assert(pool_count_available(pool) == 1);

    pool_destroy(pool);
    printf("Passes all memory tests\n");
}

int main() {
    test_ll();
    test_set();
    test_stack();
    test_bintree();
    test_math();
    test_vector();
    test_string();
    test_crypto();
    test_parsing();
    test_network();
    test_memory();

#if defined(__APPLE__) || defined(__MACH__)
    test_macos();
#endif

    return 0;
}
