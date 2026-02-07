/**
 * @file common.h
 * @brief This file contains common definitions for the project.
 */

#pragma once

/* Includes ****************************************************/

/* FreeRTOS includes. */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ESP-IDF includes. */
#include "esp_err.h"

/* Standard includes. */
#include <stdint.h>

/* Functions ****************************************************/

/**
 * @brief Initialize the completion marker.
 * @param flags All flags to set for the completion marker.
 * @return ESP_OK if the completion marker was initialized successfully, otherwise an error code.
 */
esp_err_t completion_marker_init(uint8_t flags);

/**
 * @brief Deinitialize the completion marker.
 */
void completion_marker_deinit(void);

/**
 * @brief Wait for all flags to be set in the completion marker.
 * @return ESP_OK if the completion marker was set, otherwise an error code.
 */
esp_err_t completion_marker_wait(void);

/**
 * @brief Set the completion marker.
 * @param flags The flags to set in the completion marker.
 * @return ESP_OK if the completion marker was set, otherwise an error code.
 */
esp_err_t completion_marker_set(uint8_t flags);