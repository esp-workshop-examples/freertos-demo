/**
 * @file all_examples.h
 * @brief This file contains declarations for all the examples that are available in the project.
 */

#pragma once

/* ESP-IDF includes. */
#include "esp_err.h"

/* Get the configuration header. */
#include "sdkconfig.h"

#if CONFIG_FREERTOS_DEMO_TASK_ENABLED
/**
 * @brief Run the task example.
 * @param iterations The number of iterations the task will run for.
 * @return ESP_OK if the example ran successfully, otherwise an error code.
 */
esp_err_t example_task_run(int iterations);
#endif

#if CONFIG_FREERTOS_DEMO_QUEUE_ENABLED
/**
 * @brief Run the queue example.
 * @param iterations The number of iterations the producer and consumer will produce/consume data for.
 * @return ESP_OK if the example ran successfully, otherwise an error code.
 */
esp_err_t example_queue_run(int iterations);
#endif

#if CONFIG_FREERTOS_DEMO_EVENT_GROUP_ENABLED
/**
 * @brief Run the event group example.
 * @param iterations The number of iterations the tasks will wait for the event.
 * @return ESP_OK if the example ran successfully, otherwise an error code.
 */
esp_err_t example_event_group_run(int iterations);
#endif