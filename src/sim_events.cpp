#include <Arduino.h>
#include <STM32FreeRTOS.h>

#include "sim_events.h"

#include "fsm/my_fsm.h"

static BaseType_t _sim_event_task_handle = NULL;

void _simulate_events_task(void *parameter){
  // Task logic
  Serial.println("Running myTask...");
  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

  uint8_t res = 0;

  res = my_fsm_dispatch_event(FSM_EVENT_CONNECTED);

  Serial.println("Result =" + String(res));

  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

  res = my_fsm_dispatch_event(FSM_EVENT_READY);
  
  Serial.println("Result =" + String(res));


  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

  res = my_fsm_dispatch_event(FSM_EVENT_SUSPENDED);

  Serial.println("Result =" + String(res));


  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

  res = my_fsm_dispatch_event(FSM_EVENT_SUSPENDED);

  Serial.println("Result =" + String(res));

  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

  res = my_fsm_dispatch_event(FSM_EVENT_READY);
  Serial.println("Result =" + String(res));

  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

  res = my_fsm_dispatch_event(FSM_EVENT_FINISHED);
  Serial.println("Result =" + String(res));

  vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

}

void sim_events_start_task(){
  if(_sim_event_task_handle == NULL){
    _sim_event_task_handle = xTaskCreate(_simulate_events_task,   // Function pointer to the task
                "SimulateEventsTask", // Name of the task
                configMINIMAL_STACK_SIZE,    // Stack size (in words, not bytes)
                NULL,     // Task parameter (not used in this example)
                1,        // Priority of the task (1 is default)
                NULL);    // Task handle (not used in this example)


    configASSERT(_sim_event_task_handle);

  }
}