#ifndef LIBFLINT_STATE_MACHINE_H
#define LIBFLINT_STATE_MACHINE_H

#include <stddef.h>

typedef struct {
    int id;
    void (*entry)(void *ctx, int event);
    void (*exit)(void *ctx, int event);
} SmState;

typedef struct {
    int source;
    int event;
    int destination;
    int (*guard)(void *ctx, int event);
    void (*action)(void *ctx, int event);
} SmTransition;

typedef struct {
    const SmState *state_table;
    size_t state_table_count;

    const SmTransition *transition_table;
    size_t transition_table_count;

    int current_state;
    void *ctx;
} StateMachine;

int sm_init(
    StateMachine *sm,
    const SmState *states,
    size_t state_count,
    int initial_state,
    const SmTransition *transitions,
    size_t transition_count,
    void *ctx
);

int sm_handle_event(StateMachine *sm, int event);

#define sm_current_state(sm) ((sm)->current_state)

#endif //LIBFLINT_STATE_MACHINE_H
