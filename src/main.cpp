#include <Arduino.h>
#include <STM32FreeRTOS.h>

#include <fsm/my_fsm.h>

void loop_task(void *parameter){
  const struct my_measure_params_t my_measure_params = {};
  my_measurement_fsm(&my_measure_params);

  for(;;){
    // Infinite loop
  }
}

void setup() {
  Serial.begin(9600);

  Serial.println("Starting MY FSM EXAMPLE");

  // Create a task to run the scheduler
  xTaskCreate(loop_task,     // Task function
              "LoopTask",   // Task name
              4096,              // Stack size (in words, not bytes)
              NULL,              // Task parameter
              1,                 // Priority
              NULL);             // Task handle

  
  vTaskStartScheduler();

  

  /* Should never reach this point */
	while (1) {
		Error_Handler();
	}

}

void loop() {
  // put your main code here, to run repeatedly:
}
