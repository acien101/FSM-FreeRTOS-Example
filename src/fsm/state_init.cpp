#include "my_fsm.h"
#include <STM32FreeRTOS.h>

#include "sim_events.h"

enum fsm_handler_rc state_init(struct my_fsm *fsm, struct fsm_event const *event)
{
	switch (event->type) {
  case FSM_EVENT_INIT:
		return FSM_HANDLED();
	case FSM_EVENT_ENTRY:
		//loop();
		//vTaskStartScheduler();
		sim_events_start_task();
		my_fsm_dispatch_event(FSM_EVENT_INIT_OK);
		return FSM_HANDLED();
	case FSM_EVENT_INIT_OK:
		
		return FSM_TRANSITION(&state_conn);
	case FSM_EVENT_EXIT:
		return FSM_HANDLED();
	default:
		break;
	}
	return FSM_IGNORED();
}
