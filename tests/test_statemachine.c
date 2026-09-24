/*
 * State machine test: models a simple thermostat
 *
 * States:  IDLE  ---(TEMP_LOW)--->  HEATING
 *          HEATING --(TEMP_OK)---->  IDLE
 *          * --------(FAULT)------>  ERROR
 *          ERROR ----(RESET)------>  IDLE
 *
 * The guard on TEMP_LOW ensures we only start heating if the heater
 * is not broken (ctx->heater_ok). Entry/exit callbacks track which
 * states were entered/exited. The action callback on FAULT records
 * the fault event.
 */

#include <stdio.h>
#include <string.h>
#include "lftest.h"
#include "lfstatemachine.h"

/* State and event enums */
enum {
    ST_IDLE,
    ST_HEATING,
    ST_ERROR,
    ST_COUNT
};

enum {
    EV_TEMP_LOW,
    EV_TEMP_OK,
    EV_FAULT,
    EV_RESET
};

/* Context shared between callbacks */
typedef struct {
    int heater_ok;
    int entered_heating;
    int exited_heating;
    int entered_idle;
    int entered_error;
    int fault_action_called;
} ThermoCtx;

/* Callbacks */
static void on_enter_idle(void *ctx, int event) {
    (void)event;
    ((ThermoCtx *)ctx)->entered_idle++;
}

static void on_enter_heating(void *ctx, int event) {
    (void)event;
    ((ThermoCtx *)ctx)->entered_heating++;
}

static void on_exit_heating(void *ctx, int event) {
    (void)event;
    ((ThermoCtx *)ctx)->exited_heating++;
}

static void on_enter_error(void *ctx, int event) {
    (void)event;
    ((ThermoCtx *)ctx)->entered_error++;
}

static int guard_heater_ok(void *ctx, int event) {
    (void)event;
    return ((ThermoCtx *)ctx)->heater_ok;
}

static void action_fault(void *ctx, int event) {
    (void)event;
    ((ThermoCtx *)ctx)->fault_action_called++;
}

/* Tables — this is how you'd define the FSM in real usage */
static const SmState states[] = {
    { ST_IDLE,    on_enter_idle,    NULL },
    { ST_HEATING, on_enter_heating, on_exit_heating },
    { ST_ERROR,   on_enter_error,   NULL },
};

static const SmTransition transitions[] = {
    /* source      event       dest        guard             action */
    { ST_IDLE,    EV_TEMP_LOW, ST_HEATING, guard_heater_ok,  NULL },
    { ST_HEATING, EV_TEMP_OK,  ST_IDLE,    NULL,             NULL },
    { ST_IDLE,    EV_FAULT,    ST_ERROR,   NULL,             action_fault },
    { ST_HEATING, EV_FAULT,    ST_ERROR,   NULL,             action_fault },
    { ST_ERROR,   EV_RESET,    ST_IDLE,    NULL,             NULL },
};

#define TRANSITION_COUNT (sizeof(transitions) / sizeof(transitions[0]))

static void test_basic_transitions(void) {
    ThermoCtx ctx = { .heater_ok = 1 };
    StateMachine sm;

    int ret = sm_init(&sm, states, ST_COUNT, ST_IDLE,
                      transitions, TRANSITION_COUNT, &ctx);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(sm_current_state(&sm), ST_IDLE);

    /* IDLE -> HEATING on TEMP_LOW */
    ret = sm_handle_event(&sm, EV_TEMP_LOW);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(sm_current_state(&sm), ST_HEATING);
    ASSERT_EQ(ctx.entered_heating, 1);

    /* HEATING -> IDLE on TEMP_OK */
    ret = sm_handle_event(&sm, EV_TEMP_OK);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(sm_current_state(&sm), ST_IDLE);
    ASSERT_EQ(ctx.exited_heating, 1);
    ASSERT_EQ(ctx.entered_idle, 1);
}

static void test_guard_rejects(void) {
    ThermoCtx ctx = { .heater_ok = 0 };
    StateMachine sm;

    sm_init(&sm, states, ST_COUNT, ST_IDLE, transitions, TRANSITION_COUNT, &ctx);

    /* Guard should reject — heater_ok is 0 */
    int ret = sm_handle_event(&sm, EV_TEMP_LOW);
    ASSERT_EQ(ret, 1);
    ASSERT_EQ(sm_current_state(&sm), ST_IDLE);
    ASSERT_EQ(ctx.entered_heating, 0);
}

static void test_action_callback(void) {
    ThermoCtx ctx = { .heater_ok = 1 };
    StateMachine sm;

    sm_init(&sm, states, ST_COUNT, ST_IDLE, transitions, TRANSITION_COUNT, &ctx);

    /* FAULT from IDLE should call action and enter ERROR */
    int ret = sm_handle_event(&sm, EV_FAULT);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(sm_current_state(&sm), ST_ERROR);
    ASSERT_EQ(ctx.fault_action_called, 1);
    ASSERT_EQ(ctx.entered_error, 1);
}

static void test_no_matching_transition(void) {
    ThermoCtx ctx = { .heater_ok = 1 };
    StateMachine sm;

    sm_init(&sm, states, ST_COUNT, ST_IDLE, transitions, TRANSITION_COUNT, &ctx);

    /* TEMP_OK in IDLE has no transition */
    int ret = sm_handle_event(&sm, EV_TEMP_OK);
    ASSERT_EQ(ret, 1);
    ASSERT_EQ(sm_current_state(&sm), ST_IDLE);
}

static void test_full_cycle(void) {
    ThermoCtx ctx = { .heater_ok = 1 };
    StateMachine sm;

    sm_init(&sm, states, ST_COUNT, ST_IDLE, transitions, TRANSITION_COUNT, &ctx);

    /* IDLE -> HEATING -> ERROR -> IDLE */
    sm_handle_event(&sm, EV_TEMP_LOW);
    ASSERT_EQ(sm_current_state(&sm), ST_HEATING);

    sm_handle_event(&sm, EV_FAULT);
    ASSERT_EQ(sm_current_state(&sm), ST_ERROR);
    ASSERT_EQ(ctx.exited_heating, 1);
    ASSERT_EQ(ctx.fault_action_called, 1);

    sm_handle_event(&sm, EV_RESET);
    ASSERT_EQ(sm_current_state(&sm), ST_IDLE);
}

static void test_wildcard(void) {
    ThermoCtx ctx = { .heater_ok = 1 };
    StateMachine sm;
    static const SmTransition wild[] = {
        { ST_IDLE,    EV_TEMP_LOW, ST_HEATING, guard_heater_ok, NULL },
        { ST_HEATING, EV_TEMP_OK,  ST_IDLE,    NULL,            NULL },
        { SM_ANY_STATE, EV_FAULT,  ST_ERROR,   NULL,            action_fault },
        { ST_ERROR,   EV_RESET,    ST_IDLE,    NULL,            NULL },
        { ST_IDLE,    SM_ANY_EVENT, ST_ERROR,  NULL,            NULL },
    };

    ASSERT_EQ(sm_init(&sm, states, ST_COUNT, ST_IDLE, wild, 5, &ctx), 0);

    /* specific TEMP_LOW still wins over SM_ANY_EVENT because it is first */
    ASSERT_EQ(sm_handle_event(&sm, EV_TEMP_LOW), 0);
    ASSERT_EQ(sm_current_state(&sm), ST_HEATING);

    /* FAULT from HEATING via SM_ANY_STATE */
    ASSERT_EQ(sm_handle_event(&sm, EV_FAULT), 0);
    ASSERT_EQ(sm_current_state(&sm), ST_ERROR);
    ASSERT_EQ(ctx.fault_action_called, 1);
    ASSERT_EQ(ctx.exited_heating, 1);
    ASSERT_EQ(ctx.entered_error, 1);

    ASSERT_EQ(sm_handle_event(&sm, EV_RESET), 0);
    ASSERT_EQ(sm_current_state(&sm), ST_IDLE);

    /* unmatched-at-IDLE event hits SM_ANY_EVENT */
    ASSERT_EQ(sm_handle_event(&sm, EV_TEMP_OK), 0);
    ASSERT_EQ(sm_current_state(&sm), ST_ERROR);

    ASSERT_EQ(sm_handle_event(NULL, EV_RESET), -1);

    /* destination cannot be SM_ANY_STATE */
    memset(&sm, 0x5a, sizeof(sm));
    StateMachine before = sm;
    static const SmTransition bad[] = {
        { ST_IDLE, EV_FAULT, SM_ANY_STATE, NULL, NULL },
    };
    ASSERT_EQ(sm_init(&sm, states, ST_COUNT, ST_IDLE, bad, 1, &ctx), -1);
    ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);
}

static void test_init_invalid(void) {
    StateMachine sm;

    /* NULL state machine */
    ASSERT_EQ(sm_init(NULL, states, ST_COUNT, ST_IDLE, transitions, TRANSITION_COUNT, NULL), -1);

    /* NULL states */
    ASSERT_EQ(sm_init(&sm, NULL, ST_COUNT, ST_IDLE, transitions, TRANSITION_COUNT, NULL), -1);

    /* NULL transitions */
    ASSERT_EQ(sm_init(&sm, states, ST_COUNT, ST_IDLE, NULL, TRANSITION_COUNT, NULL), -1);

    /* Zero counts */
    ASSERT_EQ(sm_init(&sm, states, 0, ST_IDLE, transitions, TRANSITION_COUNT, NULL), -1);
    ASSERT_EQ(sm_init(&sm, states, ST_COUNT, ST_IDLE, transitions, 0, NULL), -1);

    /* Invalid initial state — sm must be left untouched */
    memset(&sm, 0x5a, sizeof(sm));
    StateMachine before = sm;
    ASSERT_EQ(sm_init(&sm, states, ST_COUNT, 99, transitions, TRANSITION_COUNT, NULL), -1);
    ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);

    /* Duplicate state IDs */
    {
        const SmState dup[] = {
            { ST_IDLE, NULL, NULL },
            { ST_IDLE, NULL, NULL },
        };
        ASSERT_EQ(sm_init(&sm, dup, 2, ST_IDLE, transitions, TRANSITION_COUNT, NULL), -1);
        ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);
    }

    /* Wildcard IDs cannot also name real states. */
    {
        const SmState reserved[] = {
            { ST_IDLE, NULL, NULL },
            { SM_ANY_STATE, NULL, NULL },
        };
        const SmTransition valid[] = {
            { SM_ANY_STATE, EV_RESET, ST_IDLE, NULL, NULL },
        };
        ASSERT_EQ(sm_init(&sm, reserved, 2, SM_ANY_STATE, valid, 1, NULL), -1);
        ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);
        ASSERT_EQ(sm_init(&sm, reserved, 2, ST_IDLE, valid, 1, NULL), -1);
        ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);
    }

    /* Destination ID with no state row */
    {
        const SmTransition bad_dest[] = {
            { ST_IDLE, EV_TEMP_LOW, 99, NULL, NULL },
        };
        ASSERT_EQ(sm_init(&sm, states, ST_COUNT, ST_IDLE, bad_dest, 1, NULL), -1);
        ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);
    }

    /* Source ID with no state row */
    {
        const SmTransition bad_src[] = {
            { 99, EV_TEMP_LOW, ST_IDLE, NULL, NULL },
        };
        ASSERT_EQ(sm_init(&sm, states, ST_COUNT, ST_IDLE, bad_src, 1, NULL), -1);
        ASSERT_EQ(memcmp(&sm, &before, sizeof(sm)), 0);
    }
}

int main(void) {
    test_basic_transitions();
    test_guard_rejects();
    test_action_callback();
    test_no_matching_transition();
    test_full_cycle();
    test_wildcard();
    test_init_invalid();
    TEST_REPORT();
}
