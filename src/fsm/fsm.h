#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <time.h>

#define FSM_DEBUG_LVL_NONE   0
#define FSM_DEBUG_LVL_ERR    1
#define FSM_DEBUG_LVL_WARN   2
#define FSM_DEBUG_LVL_INFO   3
#define FSM_DEBUG_LVL_DBG    4
#define FSM_DEBUG_LVL_DBG_V  5
#define FSM_DEBUG_LVL_DBG_VV 6

extern unsigned int fsm_debug_level;

// clang-format off
#define FSM_ERR(x, ...)    do { if (fsm_debug_level >= FSM_DEBUG_LVL_ERR)    printf("[%s][ ERR]" x, ((struct fsm*)fsm)->name, ##__VA_ARGS__); } while(0)
#define FSM_WARN(x, ...)   do { if (fsm_debug_level >= FSM_DEBUG_LVL_WARN)   printf("[%s][WARN]" x, ((struct fsm*)fsm)->name, ##__VA_ARGS__); } while(0)
#define FSM_INFO(x, ...)   do { if (fsm_debug_level >= FSM_DEBUG_LVL_INFO)   printf("[%s][INFO]" x, ((struct fsm*)fsm)->name, ##__VA_ARGS__); } while(0)
#define FSM_DBG(x, ...)    do { if (fsm_debug_level >= FSM_DEBUG_LVL_DBG)    printf("[%s][ DBG]" x, ((struct fsm*)fsm)->name, ##__VA_ARGS__); } while(0)
#define FSM_DBG_V(x, ...)  do { if (fsm_debug_level >= FSM_DEBUG_LVL_DBG_V)  printf("[%s][DBGV]" x, ((struct fsm*)fsm)->name, ##__VA_ARGS__); } while(0)
#define FSM_DBG_VV(x, ...) do { if (fsm_debug_level >= FSM_DEBUG_LVL_DBG_VV) printf("[%s][DBGW]" x, ((struct fsm*)fsm)->name, ##__VA_ARGS__); } while(0)
// clang-format on

enum reserved_fsm_event_type {
	/* Reserved event types */
	FSM_EVENT_ENTRY = 0,
	FSM_EVENT_EXIT,
	FSM_EVENT_INIT,
	/* First application event, to be used as starting value for application
	 * specific event enum */
	FSM_EVENT_APPLICATION,
};

enum fsm_handler_rc {
	FSM_RC_HANDLED,
	FSM_RC_IGNORED,
	FSM_RC_TRANSITION,
};

struct fsm_event {
	uint16_t type;
};

typedef enum fsm_handler_rc (*fsm_event_handler_t)(void *fsm, struct fsm_event const *event);
typedef char *(*fsm_state_name_getter)(const fsm_event_handler_t state);
typedef uint32_t (*fsm_state_id_getter)(const fsm_event_handler_t state);
typedef char *(*fsm_event_name_getter)(const struct fsm_event *event);
typedef uint32_t (*fsm_event_id_getter)(const struct fsm_event *event);
typedef void (*fsm_transition_guard)(void *fsm, const fsm_event_handler_t next_state);

extern const struct fsm_event fsm_entry_event;
extern const struct fsm_event fsm_exit_event;
extern const struct fsm_event fsm_init_event;

#define FSM_NAME_MAX_LEN 16

struct fsm {
	fsm_event_handler_t state;

	/* Logging and tracing utilities */
	char name[FSM_NAME_MAX_LEN + 1];
	fsm_state_name_getter state_name_getter;
	int trace_transitions; // Trace transitions on UART output
	fsm_event_name_getter event_name_getter;
	int trace_events; // Trace events on UART output

	time_t prev_state_start_ts;
	fsm_state_id_getter state_id_getter;
	int log_transitions;
	fsm_event_id_getter event_id_getter;
	int log_events;
};

static inline enum fsm_handler_rc transition_guard(struct fsm *fsm, fsm_event_handler_t target)
{
	fsm->state = target;
	return FSM_RC_TRANSITION;
}

#define FSM_HANDLED()          (FSM_RC_HANDLED)
#define FSM_IGNORED()          (FSM_RC_IGNORED)
#define FSM_TRANSITION(target) transition_guard(&fsm->fsm, (fsm_event_handler_t) (target))

/* Avoid name conflict with LWIP */
void my_fsm_init(struct fsm *fsm,
                      char *name,
                      fsm_event_handler_t init_pseudo_state,
                      struct fsm_event const *init_event);
void fsm_dispatch(struct fsm *fsm, struct fsm_event const *event);

#endif /* FSM_H */
