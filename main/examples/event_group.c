/**
 * @file event_group.c
 * @brief This example demonstrates how to use FreeRTOS event groups.
 * 
 * This example uses two tasks A and B. Each task will:
 * - Wait for the other task to set their event group flag
 * - Set their own event group flag
 * - Print a message to the console
 */

/* Includes ****************************************************/

/* Declarations */
#include "all_examples.h"
#include "common.h"

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h"     // FreeRTOS core header
#include "freertos/event_groups.h" // Event group primitive
#include "freertos/task.h"         // Task primitive

/* ESP-IDF includes. */
#include "esp_log.h"

/* Types *******************************************************/

/**
 * @brief Event group data type
 * This type is used to pass data to the event group.
 */
typedef struct
{
    /* The name of the task. */
    const char *taskName;
    /* The number of iterations the task will run for. */
    int iterations;
    /* The flag to set in the event group. */
    EventBits_t flagToSet;
    /* The flag to wait for in the event group. */
    EventBits_t flagToWait;
} task_data_t;

/* Constants ***************************************************/

/* Tag for logging */
static const char *TAG = "event_group_example";

#define EVENT_GROUP_FLAG_TASK_A (1 << 0)
#define EVENT_GROUP_FLAG_TASK_B (1 << 1)
#define EVENT_GROUP_FLAGS (EVENT_GROUP_FLAG_TASK_A | EVENT_GROUP_FLAG_TASK_B)

#define EVENT_GROUP_TASK_STACK_SIZE 4096

/* Variables ***************************************************/
static task_data_t taskDataA = {    
    .taskName = "TaskA",
    .iterations = 0,
    .flagToSet = EVENT_GROUP_FLAG_TASK_A,
    .flagToWait = EVENT_GROUP_FLAG_TASK_B
};

static task_data_t taskDataB = {
    .taskName = "TaskB",
    .iterations = 0,
    .flagToSet = EVENT_GROUP_FLAG_TASK_B,
    .flagToWait = EVENT_GROUP_FLAG_TASK_A
};

/* Forward declarations *****************************************/

static void task_function(void *arg_taskData);

/* Event group (fill this in!) *********************************/

EventGroupHandle_t xEventGroup;

/**
 * @brief Initialize the event group.
 * @return ESP_OK if the event group was initialized successfully, otherwise an error code.
 */
static esp_err_t event_group_init(void)
{
    xEventGroup = xEventGroupCreate();
    if (xEventGroup == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

/**
 * @brief Deinitialize the event group.
 */
static void event_group_deinit(void)
{
    if (xEventGroup == NULL)
    {
        return;
    }
    vEventGroupDelete(xEventGroup);
    xEventGroup = NULL;
}

/** (fill this in!)
 * @brief Wait for the other task to set their event group flag.
 * @param flagToWait The flag to wait for in the event group.
 * @return ESP_OK if the other task set their event group flag, otherwise an error code.
 */
static esp_err_t event_group_wait(EventBits_t flagToWait)
{
    /* Wait on the event group for the flag to be set */
    EventBits_t eventBits = xEventGroupWaitBits(xEventGroup, flagToWait, pdTRUE, pdTRUE, portMAX_DELAY);
    if (!(eventBits & flagToWait))
    {
        return ESP_ERR_INVALID_STATE;
    }

    /* Return ESP_OK if the other task set their event group flag */
    return ESP_OK;
}

/** (fill this in!)
 * @brief Set the event group flag.
 * @param flagToSet The flag to set in the event group.
 * @return ESP_OK if the flag was set, otherwise an error code.
 */
static esp_err_t event_group_set(EventBits_t flagToSet)
{
    /* Set the event group flag */
    xEventGroupSetBits(xEventGroup, flagToSet);
    return ESP_OK;
}

/* Tasks ******************************************************/

/**
 * @brief Task function
 * This function will be executed by the task.
 */
static void task_function(void *arg_taskData)
{
    task_data_t *p_taskData = (task_data_t *)arg_taskData;
    const char *taskName = p_taskData->taskName;
    int iterations = p_taskData->iterations;
    EventBits_t flagToSet = p_taskData->flagToSet;
    EventBits_t flagToWait = p_taskData->flagToWait;

    ESP_LOGI(TAG, "[%s] Task started!", taskName);

    while (iterations > 0)
    {
        /* Log the task ID */
        ESP_LOGI(TAG, "[%s] Task ran (%d iterations left)!", p_taskData->taskName, iterations);

        /* Wait for the other task to set their event group flag */
        ESP_LOGI(TAG, "[%s] Waiting for the other task to set their event group flag!", p_taskData->taskName);
        esp_err_t err = event_group_wait(flagToWait);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "[%s] Failed to wait for the other task to set their event group flag!", p_taskData->taskName);
            break;
        }

        /* Set the event group flag */
        ESP_LOGI(TAG, "[%s] Setting the event group flag!", p_taskData->taskName);
        err = event_group_set(flagToSet);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "[%s] Failed to set the event group flag!", p_taskData->taskName);
            break;
        }
        
        /* Decrement the iterations */
        iterations--;
    }

    ESP_LOGI(TAG, "[%s] Task finished!", p_taskData->taskName);
    completion_marker_set(flagToSet);
    vTaskDelete(NULL);
}

/* Start function *********************************************/

esp_err_t example_event_group_run(int iterations)
{
    ESP_LOGI(TAG, "===== RUNNING EVENT GROUP EXAMPLE =====\n"
        "Running for %d iterations\n", iterations);

    esp_err_t err = ESP_OK;
    BaseType_t xErr;
    TaskHandle_t xTaskHandleA = NULL;
    TaskHandle_t xTaskHandleB = NULL;

    /* Initialize the completion marker */
    err = completion_marker_init(EVENT_GROUP_FLAGS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize completion marker!");
        return err;
    }

    /* Initialize the event group */
    err = event_group_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize event group!");
        return err;
    }

    /* Initialize the tasks */
    taskDataA.iterations = iterations;
    taskDataB.iterations = iterations;

    xErr = xTaskCreate(task_function, taskDataA.taskName, EVENT_GROUP_TASK_STACK_SIZE, (void*)&taskDataA, 1, &xTaskHandleA);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create task A!");
        goto example_event_group_run_fail;
    }
    xErr = xTaskCreate(task_function, taskDataB.taskName, EVENT_GROUP_TASK_STACK_SIZE, (void*)&taskDataB, 1, &xTaskHandleB);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create task B!");
        goto example_event_group_run_fail;
    }

    /* Set task B's flag to start the process */
    err = event_group_set(EVENT_GROUP_FLAG_TASK_B);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set task B's flag to start the process!");
        goto example_event_group_run_fail;
    }

    /* Wait for the tasks to finish */
    err = completion_marker_wait();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to wait for tasks to finish!");
        goto example_event_group_run_fail;
    }

    /* Deinitialize the event group */
    event_group_deinit();

    ESP_LOGI(TAG, "===== EVENT GROUP EXAMPLE FINISHED SUCCESSFULLY! =====\n");
    return ESP_OK;

example_event_group_run_fail:
    if (xTaskHandleA != NULL)
    {
        vTaskDelete(xTaskHandleA);
    }
    if (xTaskHandleB != NULL)
    {
        vTaskDelete(xTaskHandleB);
    }
    event_group_deinit();
    ESP_LOGE(TAG, "===== EVENT GROUP EXAMPLE FAILED! =====\n");
    return err;
}