#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <Arduino.h>

#include "fsm.h"

unsigned int fsm_debug_level = FSM_DEBUG_LVL_DBG_VV;

const struct fsm_event fsm_entry_event = {.type = FSM_EVENT_ENTRY};
const struct fsm_event fsm_exit_event = {.type = FSM_EVENT_EXIT};
const struct fsm_event fsm_init_event = {.type = FSM_EVENT_INIT};

static void fsm_trace_transition(const struct fsm *fsm,
                                 const fsm_event_handler_t source,
                                 const fsm_event_handler_t target)
{
	if (fsm->trace_transitions == 0 || fsm->state_name_getter == NULL) {
		return;
	}
	FSM_INFO(" %s -> %s\n", fsm->state_name_getter(source), fsm->state_name_getter(target));
}

static void fsm_log_transition(const struct fsm *fsm,
                               const fsm_event_handler_t source,
                               const fsm_event_handler_t target)
{
	// TODO implement when logging framework will be done
}

static void fsm_trace_event(const struct fsm *fsm, const struct fsm_event *event)
{
	if (fsm->trace_events == 0 || fsm->event_name_getter == NULL || fsm->state_name_getter == NULL) {
		return;
	}
	FSM_INFO(" Dispatch event %s to state handler %s\n",
	         fsm->event_name_getter(event),
	         fsm->state_name_getter(fsm->state));
}

static void fsm_log_event(const struct fsm *fsm, const struct fsm_event *event)
{
	// TODO implement when logging framework will be done
}

void my_fsm_init(struct fsm *fsm,
                      char *name,
                      const fsm_event_handler_t init_pseudo_state,
                      struct fsm_event const *init_event)
{
	struct timeval time;

	/* Initialize tracing utilities and configuration */
	strncpy(fsm->name, name, FSM_NAME_MAX_LEN);
	fsm->name[FSM_NAME_MAX_LEN] = '\0';

	fsm->state_name_getter = NULL;
	fsm->trace_transitions = 0;
	fsm->event_name_getter = NULL;
	fsm->trace_events = 0;
	fsm->state_id_getter = NULL;
	fsm->log_transitions = 0;
	fsm->event_id_getter = NULL;
	fsm->log_events = 0;

	/* Set initial pseudo state which should always transition upon getting
	 * the FSM_EVENT_INIT event type. */
	fsm->state = init_pseudo_state;

	/* Execute initial transition from initial pseudo state. */
	(*fsm->state)(fsm, init_event);
	/* Enter default state set by the initial pseudo state event handler. */
	gettimeofday(&time, NULL);
	fsm->prev_state_start_ts = time.tv_sec;
	(*fsm->state)(fsm, &fsm_entry_event);
}

static void _assert_if_transition(enum fsm_handler_rc rc)
{
	if (FSM_RC_TRANSITION == rc) {
		printf("[fsm] Forbidden transition\n");
		/* Transition are forbidden during those events
		 * - fsm_exit_event
		 * - fsm_skip_step
		 * - fsm_entry_event
		 */
		Error_Handler();
	}
}

void fsm_dispatch(struct fsm *fsm, struct fsm_event const *event)
{
	struct timeval time;
	enum fsm_handler_rc rc;
	fsm_event_handler_t prev_state = fsm->state;

	fsm_trace_event(fsm, event);
	fsm_log_event(fsm, event);
	rc = (*fsm->state)(fsm, event);

	if (FSM_RC_TRANSITION == rc) {
		fsm_trace_transition(fsm, prev_state, fsm->state);
		fsm_log_transition(fsm, prev_state, fsm->state);
		/* We are transitioning from one state to another. Send EXIT event
		 * to source state, and ENTRY event to target state. */
		_assert_if_transition((*prev_state)(fsm, &fsm_exit_event));

		gettimeofday(&time, NULL);
		fsm->prev_state_start_ts = gettimeofday(&time, NULL);
		_assert_if_transition((*fsm->state)(fsm, &fsm_entry_event));
	}
}
