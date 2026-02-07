/**
 * @file completion_marker.c
 * @brief This file contains the implementation of the completion marker.
 */

/* Includes ****************************************************/

/* Declarations */
#include "common.h"

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/* ESP-IDF includes. */
#include "esp_log.h"

/* Constants ***************************************************/

/* Tag for logging */
static const char *TAG = "completion_marker";

/* Variables ***************************************************/

/* Global variables */
static EventGroupHandle_t xCompletionMarker = NULL;
static uint8_t uxFlags = 0;

/* Functions ****************************************************/

esp_err_t completion_marker_init(uint8_t flags)
{
    xCompletionMarker = xEventGroupCreate();
    if (xCompletionMarker == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    uxFlags = flags;
    return ESP_OK;
}

void completion_marker_deinit(void)
{
    if (xCompletionMarker == NULL)
    {
        return;
    }
    vEventGroupDelete(xCompletionMarker);
    xCompletionMarker = NULL;
    uxFlags = 0;
}

esp_err_t completion_marker_wait(void)
{
    if (xCompletionMarker == NULL)
    {
        ESP_LOGE(TAG, "[COMPLETION MARKER] The completion marker has not been initialized!");
        return ESP_ERR_INVALID_STATE;
    }
    EventBits_t bits = xEventGroupWaitBits(xCompletionMarker, uxFlags, pdTRUE, pdTRUE, portMAX_DELAY);
    if (bits != uxFlags)
    {
        ESP_LOGE(TAG, "[COMPLETION MARKER] The completion marker has not been set!");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

esp_err_t completion_marker_set(uint8_t flags)
{
    if (xCompletionMarker == NULL)
    {
        ESP_LOGE(TAG, "[COMPLETION MARKER] The completion marker has not been initialized!");
        return ESP_ERR_INVALID_STATE;
    }
    xEventGroupSetBits(xCompletionMarker, flags);
    return ESP_OK;
}