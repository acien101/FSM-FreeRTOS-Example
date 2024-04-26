#ifndef MY_FSM_H
#define MY_FSM_H

#include "fsm.h"

enum my_fsm_event_type {
	FSM_EVENT_INIT_OK = FSM_EVENT_APPLICATION,
  FSM_EVENT_CONNECTED,
  FSM_EVENT_READY,
  FSM_EVENT_SUSPENDED,
  FSM_EVENT_CANCELLED,
  FSM_EVENT_FINISHED,
};

struct my_fsm {
	/* struct fsm must be the first element of this struct */
	struct fsm fsm;
};

struct my_measure_params_t {
};

int my_fsm_dispatch_event(enum my_fsm_event_type event_type);
void my_measurement_fsm(const struct my_measure_params_t *init_params);

/* State event handlers */
enum fsm_handler_rc state_init(struct my_fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_conn(struct my_fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_buffering(struct my_fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_playing(struct my_fsm *fsm, struct fsm_event const *event);
enum fsm_handler_rc state_closing(struct my_fsm *fsm, struct fsm_event const *event);


#endif /* my_FSM_H */
