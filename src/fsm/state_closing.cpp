#include "my_fsm.h"

enum fsm_handler_rc state_closing(struct my_fsm *fsm, struct fsm_event const *event)
{
	switch (event->type) {
	case FSM_EVENT_ENTRY:
		return FSM_HANDLED();
	case FSM_EVENT_EXIT:
		return FSM_HANDLED();
	default:
		break;
	}
	return FSM_IGNORED();
}
