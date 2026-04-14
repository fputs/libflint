
#include "lfstatemachine.h"

#include <stddef.h>

static const SmState *find_state(const StateMachine *sm, int id) {
    for (size_t i = 0; i < sm->state_table_count; i++) {
        if (sm->state_table[i].id == id) {
            return &sm->state_table[i];
        }
    }
    return NULL;
}

int sm_init(StateMachine *sm, const SmState *states, size_t state_count, int initial_state,
    const SmTransition *transitions, size_t transition_count, void *ctx) {
    if (sm == NULL || states == NULL || transitions == NULL || transition_count == 0 || state_count == 0) {
        return -1;
    }

    sm->state_table = states;
    sm->state_table_count = state_count;
    sm->transition_table = transitions;
    sm->transition_table_count = transition_count;
    sm->ctx = ctx;

    if (find_state(sm, initial_state) == NULL) {
        return -1;
    }
    sm->current_state = initial_state;

    return 0;
}

int sm_handle_event(StateMachine *sm, const int event) {
    for (size_t i = 0; i < sm->transition_table_count; i++) {
        if (sm->transition_table[i].source == sm_current_state(sm) && sm->transition_table[i].event == event) {
            if (sm->transition_table[i].guard != NULL) {
                if (sm->transition_table[i].guard(sm->ctx, event) == 0) {
                    continue;
                }
            }
            const SmState *src = find_state(sm, sm->current_state);
            if (src != NULL && src->exit != NULL) {
                src->exit(sm->ctx, event);
            }
            if (sm->transition_table[i].action != NULL) {
                sm->transition_table[i].action(sm->ctx, event);
            }
            sm->current_state = sm->transition_table[i].destination;
            const SmState *dst = find_state(sm, sm->current_state);
            if (dst != NULL && dst->entry != NULL) {
                dst->entry(sm->ctx, event);
            }
            return 0;
        }
    }
    return 1;
}
