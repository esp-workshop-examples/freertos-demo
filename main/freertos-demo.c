/**
 * @file freertos-demo.c
 * @brief This file is the entry point of the FreeRTOS demo application.
 */


/* Includes ****************************************************/

/* All examples */
#include "all_examples.h"

/* ESP-IDF includes. */
#include "esp_log.h"

/* Constants ****************************************************/

/* Number of iterations for each example */
#define ITERATIONS 10

/* Tag for logging */
static const char *TAG = "freertos-demo";

/* Functions ****************************************************/

void app_main(void)
{
    esp_err_t err = ESP_OK;
    /* Run the task example if enabled */
#if CONFIG_FREERTOS_DEMO_TASK_ENABLED
    err = example_task_run(ITERATIONS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Task example failed!");
    }
#endif

    /* Run the queue example if enabled */
#if CONFIG_FREERTOS_DEMO_QUEUE_ENABLED
    err = example_queue_run(ITERATIONS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Queue example failed!");
    }
#endif

    /* Run the event group example if enabled */
#if CONFIG_FREERTOS_DEMO_EVENT_GROUP_ENABLED
    ESP_LOGI(TAG, "Running event group example...");
    err = example_event_group_run(ITERATIONS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Event group example failed!");
    }
#endif
}
