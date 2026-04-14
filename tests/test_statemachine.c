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

    /* Invalid initial state */
    ASSERT_EQ(sm_init(&sm, states, ST_COUNT, 99, transitions, TRANSITION_COUNT, NULL), -1);
}

int main(void) {
    test_basic_transitions();
    test_guard_rejects();
    test_action_callback();
    test_no_matching_transition();
    test_full_cycle();
    test_init_invalid();
    TEST_REPORT();
}
