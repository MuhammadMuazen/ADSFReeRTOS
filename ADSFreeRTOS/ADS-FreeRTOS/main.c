#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//Task Priorities
#define HEAT_TASK_PRIORITY 3
#define GPS_TASK_PRIORITY 2
#define CLASSIFICATION_PRIORITY 2
#define PATH_DETERMINATION_PRIORITY 1
#define INTERCEPTION_PRIORITY 1
#define ILDE_PRIORITY 0

//Queues & Semaphores
QueueHandle_t detectionQueue;
SemaphoreHandle_t classificationSemaphore;
SemaphoreHandle_t pathDeterminationSemaphore;
SemaphoreHandle_t interceptionSemaphore;

void HeatDetectionTask(void *pvParameters);
void GPSDetectionTask(void *pvParameters);
void ClassificationTask(void *pvParameters);
void PathDeterminationTask(void *pvParameters);
void InterceptionTask(void *pvParameters);

int main() {

    printf("Initializing FreeRTOS tasks and resources...\n");

    //Create the queue for detection events
    detectionQueue = xQueueCreate(5, sizeof(char*));

    if (detectionQueue == NULL) {
        printf("Error: Failed to create detectionQueue.\n");
        return -1;
    }

    //Create semaphores
    classificationSemaphore = xSemaphoreCreateBinary();
    if (classificationSemaphore == NULL) {
        printf("Error: Failed to create classificationSemaphore.\n");
        return -1;
    }

    pathDeterminationSemaphore = xSemaphoreCreateBinary();
    if (pathDeterminationSemaphore == NULL) {
        printf("Error: Failed to create pathDeterminationSemaphore.\n");
        return -1;
    }

    interceptionSemaphore = xSemaphoreCreateBinary();
    if (interceptionSemaphore == NULL) {
	printf("Error: Failed to create interceptionSemaphore.\n");
	return -1;
    }

    //Create tasks
    xTaskCreate(HeatDetectionTask, "Heat Detection", 4096, NULL, HEAT_TASK_PRIORITY, NULL);
    xTaskCreate(GPSDetectionTask, "GPS Detection", 4096, NULL, GPS_TASK_PRIORITY, NULL);
    xTaskCreate(ClassificationTask, "Classification", 4096, NULL, CLASSIFICATION_PRIORITY, NULL);
    xTaskCreate(PathDeterminationTask, "Path Determination", 4096, NULL, PATH_DETERMINATION_PRIORITY, NULL);
    xTaskCreate(InterceptionTask, "Interception", 4096, NULL, INTERCEPTION_PRIORITY, NULL);

    printf("Starting scheduler...\n");
    vTaskStartScheduler();

    printf("Error: Scheduler failed to start.\n");

    return 0;
}


void HeatDetectionTask(void *pvParameters) {

    while(1) {

	printf("Heat Detection: Checking for objects... \n");
        vTaskDelay(pdMS_TO_TICKS(15)); //Heat detection delay

        char *event = "HeatDetection";
        xQueueSend(detectionQueue, &event, portMAX_DELAY); //Send detection event to the queue
    }
}


void GPSDetectionTask(void *pvParameters) {
    
    while(1) {

        printf("GPS Detection: Checking satellite data...\n");
        vTaskDelay(pdMS_TO_TICKS(10000)); //GPS detection delay

        char *event = "GPSDetection";
        xQueueSend(detectionQueue, &event, portMAX_DELAY);
    }
}


void ClassificationTask(void *pvParameters) {

    char *event;

    while(1) {

        if (xQueueReceive(detectionQueue, &event, portMAX_DELAY)) {
            printf("Classification: Received event - %s\n", event);

            vTaskDelay(pdMS_TO_TICKS(5000)); //Classification delay

            if (strcmp(event, "HeatDetection") == 0 || strcmp(event, "GPSDetection") == 0) {
                printf("Classification: Enemy detected. Proceeding to Path Determination.\n");
                xSemaphoreGive(classificationSemaphore);

            } else {
                printf("Classification: No threat detected. Returning to idle.\n");
            }
        }
    }
}


void PathDeterminationTask(void *pvParameters) {

    while(1) {

        if (xSemaphoreTake(classificationSemaphore, portMAX_DELAY)) {
            printf("Path Determination: Calculating interception path...\n");
            vTaskDelay(pdMS_TO_TICKS(5000)); //Path determination delay

            //Assume interception needed
            printf("Path Determination: Interception needed. Proceeding to Interception.\n");
            xSemaphoreGive(pathDeterminationSemaphore);
        }
    }
}


void InterceptionTask(void *pvParameters) {

    while(1) {

        if (xSemaphoreTake(pathDeterminationSemaphore, portMAX_DELAY)) {
            printf("Interception: Launching interception object...\n");
            vTaskDelay(pdMS_TO_TICKS(5000)); //Interception delay

            printf("Interception: Communication with object...\n");
            vTaskDelay(pdMS_TO_TICKS(1000)); //Communication delay

            //Simulate success/failure - Random value [0,1]
            bool success = (rand() % 2) == 0;

            if (success) {
                printf("Interception: Successful. Returning to idle.\n");
            } else {
                printf("Interception: Failed. Retrying...\n");
            }
        }
    }
}
