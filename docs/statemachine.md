Table-driven finite state machine with guards, entry/exit callbacks, and transition actions
## Structs

### SmState

Describes a single state and its optional entry/exit callbacks.

```c
typedef struct {
    int id;
    void (*entry)(void *ctx, int event);
    void (*exit)(void *ctx, int event);
} SmState;
```

Members:

- `id`: Integer identifying this state. Typically an enum value
- `entry`: Optional callback invoked when the machine enters this state. Receives the user context and the event that caused the transition
- `exit`: Optional callback invoked when the machine leaves this state. Receives the user context and the event that caused the transition

### SmTransition

One row of the transition table. Defines what happens when a specific event occurs in a specific state.

```c
typedef struct {
    int source;
    int event;
    int destination;
    int (*guard)(void *ctx, int event);
    void (*action)(void *ctx, int event);
} SmTransition;
```

Members:

- `source`: The state this transition applies to
- `event`: The event that triggers this transition
- `destination`: The state to move to
- `guard`: Optional function that must return nonzero for the transition to proceed. If `NULL`, the transition is unconditional
- `action`: Optional function called during the transition, after the source state's exit callback and before the destination state's entry callback

### StateMachine

The state machine instance.

```c
typedef struct {
    const SmState *state_table;
    size_t state_table_count;

    const SmTransition *transition_table;
    size_t transition_table_count;

    int current_state;
    void *ctx;
} StateMachine;
```

Members:

- `state_table`: Pointer to an array of `SmState` definitions
- `state_table_count`: Number of entries in `state_table`
- `transition_table`: Pointer to an array of `SmTransition` definitions
- `transition_table_count`: Number of entries in `transition_table`
- `current_state`: The ID of the current state
- `ctx`: User-provided context pointer passed to all callbacks

## Functions

### sm_init

Initializes the state machine with the given state and transition tables. The `initial_state` must match the `id` of a state in the `states` array. State IDs must be unique. Every transition `source` must be a state id or `SM_ANY_STATE`. Every `destination` must name a real state; `SM_ANY_STATE` is not a valid destination. Validation runs before any write. A failed `sm_init` leaves `sm` untouched. The initial state's `entry` callback is not invoked. Returns `0` on success, `-1` on error (NULL arguments, zero counts, duplicate state IDs, or unknown state IDs).

```c
int sm_init(
    StateMachine *sm,
    const SmState *states,
    size_t state_count,
    int initial_state,
    const SmTransition *transitions,
    size_t transition_count,
    void *ctx
);

/* Usage */
enum { ST_IDLE, ST_RUNNING, ST_ERROR, ST_COUNT };
enum { EV_START, EV_STOP, EV_FAULT };

const SmState states[] = {
    { ST_IDLE,    on_enter_idle, NULL },
    { ST_RUNNING, on_enter_run,  on_exit_run },
    { ST_ERROR,   on_enter_err,  NULL },
};

const SmTransition transitions[] = {
    { ST_IDLE,    EV_START, ST_RUNNING, NULL,          NULL },
    { ST_RUNNING, EV_STOP,  ST_IDLE,    NULL,          NULL },
    { ST_RUNNING, EV_FAULT, ST_ERROR,   NULL,          log_fault },
    { ST_ERROR,   EV_STOP,  ST_IDLE,    guard_cleared, NULL },
};

StateMachine sm;
sm_init(&sm, states, ST_COUNT, ST_IDLE,
        transitions, 4, &my_context);
```

### sm_handle_event

Processes an event. Scans the transition table for the first entry matching the current state and event whose guard (if any) returns nonzero. `SM_ANY_STATE` matches any current state. `SM_ANY_EVENT` matches any event. Put specific rows before wildcards; the first match wins. When a match is found, the machine runs:

1. Source state's `exit` callback
2. Transition's `action` callback
3. State change to `destination`
4. Destination state's `entry` callback

Returns `0` if a transition was taken, `1` if no matching transition was found (state unchanged), `-1` if `sm` is NULL.

```c
int sm_handle_event(StateMachine *sm, int event);

/* Usage: typical super-loop */
while (1) {
    int event = poll_events();
    sm_handle_event(&sm, event);
}
```

## Macros

### SM_ANY_STATE / SM_ANY_EVENT

Wildcard source and event. Destination cannot be `SM_ANY_STATE`. Values are `INT_MIN`; do not use that as a real state or event id.

```c
#define SM_ANY_STATE INT_MIN
#define SM_ANY_EVENT INT_MIN

/* Usage: one FAULT row instead of one per state */
{ SM_ANY_STATE, EV_FAULT, ST_ERROR, NULL, log_fault },
```

### sm_current_state

Returns the current state ID.

```c
#define sm_current_state(sm) ((sm)->current_state)
```

## Design Notes

**Transition priority**: When multiple transitions match the same state and event (e.g., with different guards), the first match in the table wins. Order your transition table accordingly.

**Self-transitions**: A transition whose `destination` equals `source` is still an external transition: `exit`, `action`, and `entry` all run.

**No re-entry from callbacks**: `entry`, `exit`, `action`, and `guard` must not call `sm_handle_event` on the same machine.

**Allocation-free**: The state machine performs no dynamic allocation. Both tables can be `const` and placed in flash/ROM on embedded targets. The `StateMachine` struct itself can live on the stack or in static memory.

**State lookup**: States are looked up by `id`, not by array index. State IDs do not need to be contiguous or zero-based, though contiguous enums are recommended for simplicity.
