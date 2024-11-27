/*
*	sensors.c --> This file will holds the Heat sensor task and the GPS sensor function task
*/
#include "ADS/ADS.h"
#include "FreeRTOSConfig.h"
#include "freertos/projdefs.h"

/*
	Function that represent the heat sensor task
	Input --> pvParam as void* which represent the parameters of the task
	Output --> void
	Time --> 15 ms
*/
void vHeatSensorTask(void *pvParam) {
	
	while(1) {
		
		printf("Heat Sensor is working!\n");
		
		vTaskDelay(pdMS_TO_TICKS(15));
	}
}
