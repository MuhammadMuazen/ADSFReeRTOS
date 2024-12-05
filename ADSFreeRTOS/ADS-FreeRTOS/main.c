#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdlib.h>

//Task Priorities
#define HEAT_TASK_PRIORITY 3 // T1
#define GPS_TASK_PRIORITY 3  // T2
#define CLASSIFICATION_PRIORITY 1 // T3
#define PATH_DETERMINATION_PRIORITY 1 // T4
#define LAUNCHING_DEFENSIVE_OBJECT_PRIORITY 3 // T5
#define CONNECTING_WITH_DEFENSIVE_OBJECT_PRIORITY 2 // T6
#define STARTING_ALERTS_PRIORITY 0 // T7

// Task Delays
#define HEAT_DETECTION_DELAY pdMS_TO_TICKS(15)
#define GPS_DETECTION_DELAY pdMS_TO_TICKS(10000)
#define CLASSIFICATION_DELAY pdMS_TO_TICKS(5000)
#define PATH_DETEREMNATION_DELAY pdMS_TO_TICKS(5000)
#define LAUNCHING_DEFENSE_OBJECT_DELAY pdMS_TO_TICKS(5000)
#define CONNECT_WITH_THE_DEFENSIVE_OBJECT_DELAY pdMS_TO_TICKS(1000)
#define STARTING_ALERTS_DELAY pdMS_TO_TICKS(2000)

// Tasks functions prototypes
void HeatDetectionTask(void *pvParameters);
void GPSDetectionTask(void *pvParameters);
void ClassificationTask(void *pvParameters);
void PathDeterminationTask(void *pvParameters);
void LaunchingDefensiveObject(void *pvParameters);
void ConnectingWithDefensiveObject(void* pvParameters);
void StartingAlerts(void *pvParameters);

// Task creation mutex
SemaphoreHandle_t task_creation_mutex;
// Keep track of the alerts state
int alerts_on = 0;
SemaphoreHandle_t alerts_mutex;


int main() {

	srand(time(NULL));

    printf("[+] Initializing FreeRTOS tasks and resources...\n");

	// Initialize mutexes
    alerts_mutex = xSemaphoreCreateMutex();
    task_creation_mutex = xSemaphoreCreateMutex();

    if (alerts_mutex == NULL || task_creation_mutex == NULL) {
        printf("[!] Error: Mutex creation failed.\n");
        return -1;
    }

    //Heat and GPS tasks creation
    xTaskCreate(HeatDetectionTask, "Heat Detection", 4096, NULL, HEAT_TASK_PRIORITY, NULL);
    xTaskCreate(GPSDetectionTask, "GPS Detection", 4096, NULL, GPS_TASK_PRIORITY, NULL);

    printf("Starting scheduler...\n");
    vTaskStartScheduler();

    printf("Error: Scheduler failed to start.\n");

    return 0;
}


void HeatDetectionTask(void *pvParameters) {

	printf("[+] Heat Detection: Checking for objects... \n");

    while(1) {

    	// If this value is 1 it means that there is an object detected (Dump implementation BTW)
    	int heat_object_detected = rand() % 2;

    	if(heat_object_detected) {

    		printf("[!] Object detected using heat detector!\n");

            xSemaphoreTake(task_creation_mutex, portMAX_DELAY);
            xTaskCreate(ClassificationTask, "Classification", 4096, NULL, CLASSIFICATION_PRIORITY, NULL);
            xSemaphoreGive(task_creation_mutex);

    	} else {
    		printf("[-] Heat detection did not catch any object...\n");
    	}

        vTaskDelay(HEAT_DETECTION_DELAY);
    }
}


void GPSDetectionTask(void *pvParameters) {
    
	printf("[+] GPS Detection: Checking satellite data...\n");

    while(1) {

    	// If this value is 1 it means that there is an object detected
    	int gps_object_detected = rand() % 2;

    	if(gps_object_detected) {

    		printf("[!] Object detected using GPS detector!\n");

            xSemaphoreTake(task_creation_mutex, portMAX_DELAY);
            xTaskCreate(ClassificationTask, "Classification", 4096, NULL, CLASSIFICATION_PRIORITY, NULL);
            xSemaphoreGive(task_creation_mutex);

    	} else {
    		printf("[-] GPS detection could not detect any object...\n");
    	}

        vTaskDelay(GPS_DETECTION_DELAY);
    }
}


void ClassificationTask(void *pvParameters) {

	printf("[+] Classifying Object...\n");

	while(1) {

		// If this value is 1 it means the object we detected is enemey object
		int is_enemy = rand() % 2;

		if(is_enemy) {

			printf("[!] Object is enenmy!\n");

    	    xTaskCreate(PathDeterminationTask, "Path Determination", 4096, NULL, PATH_DETERMINATION_PRIORITY, NULL);

		} else {
			printf("[-] Detected object is not an enemy!");
		}

		vTaskDelay(CLASSIFICATION_DELAY);
	}
}


void PathDeterminationTask(void *pvParameters) {

	printf("[+] Detecting if the object worth intercepting...\n");

	while(1) {

		int need_interception = rand() % 2;

		if(need_interception) {

			printf("[!] Interception Needed!\n");

			xSemaphoreTake(task_creation_mutex, portMAX_DELAY);
			xTaskCreate(LaunchingDefensiveObject, "LauncingDefensiveObject", 4096, NULL, LAUNCHING_DEFENSIVE_OBJECT_PRIORITY, NULL);
            xSemaphoreGive(task_creation_mutex);
		}

		vTaskDelay(PATH_DETEREMNATION_DELAY);
	}
}


void LaunchingDefensiveObject(void *pvParameters) {

	printf("[!] Launching the defensive object...\n");

	while(1) {

		xTaskCreate(ConnectingWithDefensiveObject, "ConnectingWithDefensiveObject", 4096, NULL, CONNECTING_WITH_DEFENSIVE_OBJECT_PRIORITY, NULL);

		vTaskDelay(LAUNCHING_DEFENSE_OBJECT_DELAY);
	}

}

void ConnectingWithDefensiveObject(void* pvParameters) {

	printf("[+] Connecting with the defensive object...\n");

	while(1) {

		int intercept_failed = rand() % 2;

		xSemaphoreTake(alerts_mutex, portMAX_DELAY);
 		if(!alerts_on) {


 			printf("[!] Starting Alerts...\n");

 			xTaskCreate(StartingAlerts, "StartingAlerts", 4096, NULL, STARTING_ALERTS_PRIORITY, NULL);

 			alerts_on = 1;
 		}
		xSemaphoreGive(alerts_mutex);

		if(intercept_failed) {

			printf("[-] Intercept failed launching another defensive object...");
			
			xSemaphoreTake(task_creation_mutex, portMAX_DELAY);
			xTaskCreate(LaunchingDefensiveObject, "LauncingDefensiveObject", 4096, NULL, LAUNCHING_DEFENSIVE_OBJECT_PRIORITY, NULL);
            xSemaphoreGive(task_creation_mutex);

		} else {
            printf("[+] Interception succeeded!\n");
            
			xSemaphoreTake(alerts_mutex, portMAX_DELAY);
            alerts_on = 0;
            xSemaphoreGive(alerts_mutex);
		}

		vTaskDelay(CONNECT_WITH_THE_DEFENSIVE_OBJECT_DELAY);
	}
}

void StartingAlerts(void *pvParameters) {

	while(1) {

		printf("[+] Alerts are ON\n");

		vTaskDelay(STARTING_ALERTS_DELAY);
	}
}
