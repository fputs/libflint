#include "lfstatemachine.h"

#include <stddef.h>

static const SmState *find_state_in(const SmState *states, size_t state_count, int id) {
    for (size_t i = 0; i < state_count; i++) {
        if (states[i].id == id) {
            return &states[i];
        }
    }
    return NULL;
}

static const SmState *find_state(const StateMachine *sm, int id) {
    return find_state_in(sm->state_table, sm->state_table_count, id);
}

static int states_unique(const SmState *states, size_t state_count) {
    for (size_t i = 0; i < state_count; i++) {
        for (size_t j = i + 1; j < state_count; j++) {
            if (states[i].id == states[j].id) {
                return 0;
            }
        }
    }
    return 1;
}

int sm_init(StateMachine *sm, const SmState *states, size_t state_count, int initial_state,
    const SmTransition *transitions, size_t transition_count, void *ctx) {
    if (sm == NULL || states == NULL || transitions == NULL || transition_count == 0 || state_count == 0) {
        return -1;
    }
    if (!states_unique(states, state_count)) {
        return -1;
    }
    if (find_state_in(states, state_count, initial_state) == NULL) {
        return -1;
    }
    for (size_t i = 0; i < transition_count; i++) {
        int src = transitions[i].source;
        int dst = transitions[i].destination;
        if ((src != SM_ANY_STATE &&
             find_state_in(states, state_count, src) == NULL) ||
            dst == SM_ANY_STATE ||
            find_state_in(states, state_count, dst) == NULL) {
            return -1;
        }
    }

    sm->state_table = states;
    sm->state_table_count = state_count;
    sm->transition_table = transitions;
    sm->transition_table_count = transition_count;
    sm->ctx = ctx;
    sm->current_state = initial_state;
    return 0;
}

int sm_handle_event(StateMachine *sm, const int event) {
    if (sm == NULL) {
        return -1;
    }
    for (size_t i = 0; i < sm->transition_table_count; i++) {
        int src_ok = (sm->transition_table[i].source == sm_current_state(sm) ||
                      sm->transition_table[i].source == SM_ANY_STATE);
        int ev_ok = (sm->transition_table[i].event == event ||
                     sm->transition_table[i].event == SM_ANY_EVENT);
        if (src_ok && ev_ok) {
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
