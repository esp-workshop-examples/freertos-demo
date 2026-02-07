/**
 * @file queue.c
 * @brief This example demonstrates how to use a queue across two tasks.
 *
 * This example uses two tasks Producer and Consumer:
 * - The Producer task will send data to the queue every second.
 * - The Consumer task will receive data from the queue every second.
 */

/* Includes ****************************************************/

/* Declarations */
#include "all_examples.h"
#include "common.h"

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h" // FreeRTOS core header
#include "freertos/queue.h"    // Queue primitive
#include "freertos/task.h"     // Task primitive

/* ESP-IDF includes. */
#include "esp_log.h"

/* Data types ***************************************************/

/**
 * @brief Data type for the queue.
 * This is the data that is transferred between the producer and the consumer.
 */
typedef struct
{
    /* An integer */
    int demo_int;
    /* A float */
    float demo_float;
    /* A string */
    const char *demo_string;
} queue_data_t;

/* Constants ***************************************************/

/* The size of the queue */
#define QUEUE_SIZE 10

/* The stack size of the tasks */
#define QUEUE_TASK_STACK_SIZE 2048

/* Tag for logging */
static const char *TAG = "queue_example";

/* Completion marker flags */
#define COMPLETION_MARKER_FLAG_PRODUCER_ENDED (1 << 0)
#define COMPLETION_MARKER_FLAG_CONSUMER_ENDED (1 << 1)
#define COMPLETION_MARKER_FLAGS (COMPLETION_MARKER_FLAG_PRODUCER_ENDED | COMPLETION_MARKER_FLAG_CONSUMER_ENDED)

/* Forward declarations *****************************************/

static void task_producer(void *arg_iterations);
static void task_consumer(void *arg_iterations);

static esp_err_t queue_init(void);
static void queue_deinit(void);
static esp_err_t queue_send_and_block_if_full(const queue_data_t *p_data);
static esp_err_t queue_receive_and_block_if_empty(queue_data_t *p_data);

/* Tasks ******************************************************/

/**
 * @brief Producer task
 * This task will send data to the queue every second.
 */
static void task_producer(void *arg_iterations)
{
    int iterations = (int)arg_iterations;
    int demo_int = 0;
    float demo_float = 0.0f;
    const char *demo_string = "Hello, World!";
    while (iterations > 0)
    {
        /* Construct some data to send to the queue */
        demo_int++;
        demo_float += 0.1f;
        queue_data_t data = {
            .demo_int = demo_int,
            .demo_float = demo_float,
            .demo_string = demo_string
        };

        /* Send the data to the queue */
        esp_err_t err = queue_send_and_block_if_full(&data);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "[QUEUE] Failed to send data to the queue!");
            break;
        }

        /* Delay for 1 second */
        vTaskDelay(pdMS_TO_TICKS(1000));
        iterations--;
    }

    ESP_LOGI(TAG, "[QUEUE] Producer task finished!");
    completion_marker_set(COMPLETION_MARKER_FLAG_PRODUCER_ENDED);
    vTaskDelete(NULL);
}

/**
 * @brief Consumer task
 * This task will receive data from the queue every second.
 */
static void task_consumer(void *arg_iterations)
{
    int iterations = (int)arg_iterations;
    while (iterations > 0)
    {
        queue_data_t data;

        /* Receive the data from the queue */
        esp_err_t err = queue_receive_and_block_if_empty(&data);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "[QUEUE] Failed to receive data from the queue!");
            break;
        }

        /* Delay for 1 second */
        vTaskDelay(pdMS_TO_TICKS(1000));
        iterations--;
    }

    ESP_LOGI(TAG, "[QUEUE] Consumer task finished!");
    completion_marker_set(COMPLETION_MARKER_FLAG_CONSUMER_ENDED);
    vTaskDelete(NULL);
}

/* Queue (fill this in!) ***************************************/

QueueHandle_t xQueue;

static esp_err_t queue_init(void)
{
    /* Initialize the queue into the global variable xQueue with the size QUEUE_SIZE */
    xQueue = xQueueCreate(QUEUE_SIZE, sizeof(queue_data_t));
    if (xQueue == NULL)
    {
        ESP_LOGE(TAG, "[QUEUE] Failed to create queue!");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static void queue_deinit(void)
{
    if (xQueue == NULL)
    {
        return;
    }
    vQueueDelete(xQueue);
    xQueue = NULL;
}

static esp_err_t queue_send_and_block_if_full(const queue_data_t *p_data)
{
    if (p_data == NULL || p_data->demo_string == NULL)
    {
        ESP_LOGE(TAG, "[QUEUE] Invalid data!");
        return ESP_ERR_INVALID_ARG;
    }

    /* Log the data to send */
    ESP_LOGI(TAG, "[QUEUE] Sending data:\n"
        "    demo_int: %d\n"
        "    demo_float: %f\n"
        "    demo_string: %s",
        p_data->demo_int,
        p_data->demo_float,
        p_data->demo_string);

    /* Send the data to the queue and block if the queue is full */
    BaseType_t xErr = xQueueSend(xQueue, p_data, portMAX_DELAY);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "[QUEUE] Failed to send data to the queue!");
        return ESP_FAIL;
    }   

    /* Return ESP_OK if the data was sent successfully */
    return ESP_OK;
}

static esp_err_t queue_receive_and_block_if_empty(queue_data_t *p_data)
{
    /* Wait for the queue to have data and then receive the data from the queue, then store it in the p_data parameter */
    BaseType_t xErr = xQueueReceive(xQueue, p_data, portMAX_DELAY);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "[QUEUE] Failed to receive data from the queue!");
        return ESP_FAIL;
    }

    /* Log the received data */
    ESP_LOGI(TAG, "[QUEUE] Received data:\n"
        "    demo_int: %d\n"
        "    demo_float: %f\n"
        "    demo_string: %s",
        p_data->demo_int,
        p_data->demo_float,
        p_data->demo_string);

    /* Return ESP_OK if the data was received successfully */
    return ESP_OK;
}

/* Start function *********************************************/

esp_err_t example_queue_run(int iterations)
{
    ESP_LOGI(TAG, "===== RUNNING QUEUE EXAMPLE =====\n"
        "Running for %d iterations\n"
        "Queue size: %d\n",
        iterations,
        (int)QUEUE_SIZE);

    esp_err_t err = ESP_OK;
    BaseType_t xErr;
    TaskHandle_t xTaskHandleProducer = NULL;
    TaskHandle_t xTaskHandleConsumer = NULL;

    /* Initialize the completion marker */
    err = completion_marker_init(COMPLETION_MARKER_FLAGS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize completion marker!");
        goto example_queue_run_fail;
    }

    /* Initialize the queue */
    err = queue_init();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize queue!");
        goto example_queue_run_fail;
    }

    /* Start the tasks */
    xErr = xTaskCreate(task_producer, "QueueProducer", QUEUE_TASK_STACK_SIZE, (void*)iterations, 1, &xTaskHandleProducer);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create producer task!");
        goto example_queue_run_fail;
    }
    xErr = xTaskCreate(task_consumer, "QueueConsumer", QUEUE_TASK_STACK_SIZE, (void*)iterations, 1, &xTaskHandleConsumer);
    if (xErr != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create consumer task!");
        goto example_queue_run_fail;
    }

    /* Wait for the tasks to finish */
    err = completion_marker_wait();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to wait for tasks to finish!");
        goto example_queue_run_fail;
    }

    /* Deinitialize the queue */
    queue_deinit();

    /* Deinitialize the completion marker */
    completion_marker_deinit();

    ESP_LOGI(TAG, "===== QUEUE EXAMPLE FINISHED SUCCESSFULLY! =====\n");
    return ESP_OK;

example_queue_run_fail:
    if (xTaskHandleProducer != NULL)
    {
        vTaskDelete(xTaskHandleProducer);
    }
    if (xTaskHandleConsumer != NULL)
    {
        vTaskDelete(xTaskHandleConsumer);
    }
    queue_deinit();
    completion_marker_deinit();
    ESP_LOGE(TAG, "!!!!!! QUEUE EXAMPLE FAILED !!!!!!");
    return err;
}