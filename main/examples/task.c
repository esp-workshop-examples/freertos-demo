/**
 * @file task.c
 * @brief This example demonstrates how to use FreeRTOS tasks.
 */

/* Includes ****************************************************/

/* Declarations */
#include "all_examples.h"
#include "common.h"

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h" // FreeRTOS core header
#include "freertos/task.h"     // Task primitive

/* ESP-IDF includes. */
#include "esp_log.h"

/* Types *******************************************************/

/**
 * @brief Task data type
 * This type is used to pass data to the task.
 */
typedef struct
{
    int taskId;
    int iterations;
} task_data_t;

/* Constants ***************************************************/

/* Tag for logging */
static const char *TAG = "task_example";

/* Make 8 tasks since we have 8 completion marker flags */
#define TASK_COUNT 8

/* The stack size of the tasks */
#define TASK_STACK_SIZE 2048

/* Variables ***************************************************/
static task_data_t taskData[TASK_COUNT];

/* Forward declarations *****************************************/

static esp_err_t task_create(TaskHandle_t *p_TaskHandle, int taskId, int iterations);
static void task_function(void *arg_taskData);

/* Tasks ******************************************************/

/** (fill this in!)
 * @brief Create a task
 * This function will create a task and return the task handle.
 * @param p_TaskHandle The task handle to return.
 * @param taskId The ID of the task.
 * @param iterations The number of iterations the task will run for.
 * @return ESP_OK if the task was created successfully, otherwise an error code.
 */
static esp_err_t task_create(TaskHandle_t *p_TaskHandle, int taskId, int iterations)
{
    /**
     * Create the task data.
     * This is done with a static memory location because the argument passed to the task function must be valid for the lifetime of the task.
     */
    taskData[taskId] = (task_data_t){
        .taskId = taskId,
        .iterations = iterations
    };

    /* Create the task */
    char taskName[15];
    snprintf(taskName, sizeof(taskName), "Task%d", taskId);
    BaseType_t xErr = xTaskCreate(task_function, taskName, TASK_STACK_SIZE, &taskData[taskId], 1, p_TaskHandle);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "[TASK] Failed to create task %d!", taskId);
        return ESP_FAIL;
    }

    /* Return ESP_OK if the task was created successfully */
    return ESP_OK;
}

/**
 * @brief Task function
 * This function will be executed by the task.
 */
static void task_function(void *arg_taskData)
{
    task_data_t *p_taskData = (task_data_t *)arg_taskData;
    int taskId = p_taskData->taskId;
    int iterations = p_taskData->iterations;

    ESP_LOGI(TAG, "[TASK] Task %d started!", taskId);

    while (iterations > 0)
    {
        /* Log the task ID */
        ESP_LOGI(TAG, "[TASK] Task %d ran (%d iterations left)!", taskId, iterations);

        /* Delay for taskId * 100 milliseconds */
        int delay = (taskId + 1) * 100;
        vTaskDelay(pdMS_TO_TICKS(delay));

        /* Decrement the iterations */
        iterations--;
    }

    ESP_LOGI(TAG, "[TASK] Task %d finished!", taskId);
    completion_marker_set(1 << taskId);
    vTaskDelete(NULL);
}



/* Start function *********************************************/

esp_err_t example_task_run(int iterations)
{
    ESP_LOGI(TAG, "===== RUNNING TASK EXAMPLE =====\n"
        "Running each task for %d iterations!\n", iterations);

    esp_err_t err = ESP_OK;
    TaskHandle_t xTaskHandles[TASK_COUNT] = { NULL };

    /* Initialize the completion marker */
    uint32_t completion_marker_flags = (1 << TASK_COUNT) - 1;
    err = completion_marker_init(completion_marker_flags);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize completion marker!");
        goto example_task_run_fail;
    }

    /* Initialize the tasks */
    for (int i = 0; i < TASK_COUNT; i++)
    {
        err = task_create(&xTaskHandles[i], i, iterations);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to create task %d!", i);
            goto example_task_run_fail;
        }
    }

    /* Wait for the tasks to finish */
    err = completion_marker_wait();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to wait for tasks to finish!");
        goto example_task_run_fail;
    }

    /* Deinitialize the completion marker */
    completion_marker_deinit();

    ESP_LOGI(TAG, "===== TASK EXAMPLE FINISHED SUCCESSFULLY! =====\n");
    return ESP_OK;

example_task_run_fail:
    for (int i = 0; i < TASK_COUNT; i++)
    {
        if (xTaskHandles[i] != NULL)
        {
            vTaskDelete(xTaskHandles[i]);
        }
    }
    completion_marker_deinit();
    ESP_LOGE(TAG, "!!!!!! TASK EXAMPLE FAILED !!!!!!");
    return err;
}