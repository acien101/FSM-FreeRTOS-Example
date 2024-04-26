#include "my_fsm.h"

enum fsm_handler_rc state_conn(struct my_fsm *fsm, struct fsm_event const *event)
{
	switch (event->type) {
	case FSM_EVENT_ENTRY:
		return FSM_HANDLED();
	case FSM_EVENT_EXIT:
		return FSM_HANDLED();
  case FSM_EVENT_CONNECTED:
    return FSM_TRANSITION(&state_buffering);
	default:
		break;
	}
	return FSM_IGNORED();
}
