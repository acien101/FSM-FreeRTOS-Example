#include "my_fsm.h"

#include <Arduino.h>
#include <STM32FreeRTOS.h>

static QueueHandle_t event_queue = NULL;

int my_fsm_dispatch_event(enum my_fsm_event_type event_type)
{
	struct fsm_event event = {.type = event_type};

	if (event_queue == NULL) {
		return -1;
	}

	if (xQueueSend(event_queue, (void *) &event, 0) != pdPASS) {
		/* Queue full, abnormal, to be investigated */
		Error_Handler();
		return -1;
	}

	return 0;
}

enum my_fsm_state_id {
	MY_FSM_STATE_INIT,
	MY_FSM_STATE_CONNECTING,
	MY_FSM_STATE_BUFFERING,
	MY_FSM_STATE_PLAYING,
	MY_FSM_STATE_CLOSING
};

struct my_fsm_state_mapping {
	fsm_handler_rc (*state)(struct my_fsm*, const fsm_event*);;
	enum my_fsm_state_id id;
	char name[32];
};

// clang-format off
struct my_fsm_state_mapping my_fsm_state_map[] = {
		{state_init,                  MY_FSM_STATE_INIT,                  "state_init"},
    {state_conn,                  MY_FSM_STATE_CONNECTING,                  "state_connecting"},
    {state_buffering,                MY_FSM_STATE_BUFFERING,                "state_buffering"},
    {state_playing,                  MY_FSM_STATE_PLAYING,                  "state_playing"},
    {state_closing,                  MY_FSM_STATE_CLOSING,                  "state_closing"},
    {NULL},
};
// clang-format on

char *my_fsm_state_name_getter(const fsm_event_handler_t state)
{
	struct my_fsm_state_mapping *mapping = my_fsm_state_map;

	while (mapping->state != NULL) {
		if ((void *) state == mapping->state) {
			return mapping->name;
		}

		mapping++;
	}

	Error_Handler();
	return "UNKNOWN_STATE";
}

uint32_t my_fsm_state_id_getter(const fsm_event_handler_t state)
{
	struct my_fsm_state_mapping *mapping = my_fsm_state_map;

	while (mapping->state != NULL) {
		if ((void *) state == mapping->state) {
			return mapping->id;
		}

		mapping++;
	}

	Error_Handler();
	return 0;
}

char *my_fsm_event_name_getter(const struct fsm_event *event)
{
	switch (event->type) {
	case FSM_EVENT_ENTRY:
		return "ENTRY";
	case FSM_EVENT_EXIT:
		return "EXIT";
	case FSM_EVENT_INIT:
		return "INIT";
	case FSM_EVENT_INIT_OK:
		return "FSM_EVENT_INIT_OK";
	case FSM_EVENT_CONNECTED:
		return "FSM_EVENT_CONNECTED";
	case FSM_EVENT_READY:
		return "FSM_EVENT_READY";
	case FSM_EVENT_SUSPENDED:
		return "FSM_EVENT_SUSPENDED";
	case FSM_EVENT_CANCELLED:
		return "FSM_EVENT_CANCELLED";
	case FSM_EVENT_FINISHED:
		return "FSM_EVENT_FINISHED";
	default:
		return "UNKNOWN_EVT";
	}
}

void my_measurement_fsm(const struct my_measure_params_t *init_params)
{
	struct fsm_event fsm_event;

	struct fsm *fsm;
	struct my_fsm my_fsm = {0};

	fsm = (struct fsm *) &my_fsm;

	/* Create event queue and run event loop */
	event_queue = xQueueCreate(8, sizeof(struct fsm_event));
	if (event_queue == NULL) {
		printf("[my] Could not create event queue for my_FSM\n");
		Error_Handler();
		return;
	}

	my_fsm_init(fsm, "my_FSM", (fsm_event_handler_t) &state_init, &fsm_init_event);

	/* Configure tracing on state transitions and events */
	fsm->state_name_getter = my_fsm_state_name_getter;
	fsm->trace_transitions = 1;
	fsm->event_name_getter = my_fsm_event_name_getter;
	fsm->trace_events = 1;
	fsm->state_id_getter = my_fsm_state_id_getter;
	fsm->log_transitions = 1;
	fsm->log_events = 1;

	while (1) {
		xQueueReceive(event_queue, &fsm_event, portMAX_DELAY);
		fsm_dispatch(fsm, &fsm_event);

		if (fsm->state == (fsm_event_handler_t) state_closing) {
			break;
		}
	}

	vQueueDelete(event_queue);
}
