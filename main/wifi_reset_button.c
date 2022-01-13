/*
 * wifi_reset_button.c
 *
 *  Created on: Nov 1, 2021
 *      Author: kjagu
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"

static const char TAG[] = "wifi_reset_button";

// Semaphore handle
SemaphoreHandle_t wifi_reset_semphore = NULL;

/**
 * ISR handler for the Wifi reset (BOOT) button
 * @param arg parameter which can be passed to the ISR handler.
 */
void IRAM_ATTR wifi_reset_button_isr_handler(void *arg)
{
	// Notify the button task
	xSemaphoreGiveFromISR(wifi_reset_semphore, NULL);
}

/**
 * Wifi reset button task reacts to a BOOT button event by sending a message
 * to the Wifi application to disconnect from Wifi and clear the saved credentials.
 * @param pvParam parameter which can be passed to the task.
 */
void wifi_reset_button_task(void *pvParam)
{
	for (;;)
	{
		if (xSemaphoreTake(wifi_reset_semphore, portMAX_DELAY) ==  pdTRUE)
		{
			ESP_LOGI(TAG, "WIFI RESET BUTTON INTERRUPT OCCURRED");

			// Send a message to disconnect Wifi and clear credentials
			wifi_app_send_message(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);

			vTaskDelay(2000 / portTICK_PERIOD_MS);
		}
	}
}

void wifi_reset_button_config(void)
{
	// Create the binary semaphore
	wifi_reset_semphore = xSemaphoreCreateBinary();

	// Configure the button and set the direction
	gpio_pad_select_gpio(WIFI_RESET_BUTTON);
	gpio_set_direction(WIFI_RESET_BUTTON, GPIO_MODE_INPUT);

	// Enable interrupt on the negative edge
	gpio_set_intr_type(WIFI_RESET_BUTTON, GPIO_INTR_NEGEDGE);

	// Create the Wifi reset button task
	xTaskCreatePinnedToCore(&wifi_reset_button_task, "wifi_reset_button", WIFI_RESET_BUTTON_TASK_STACK_SIZE, NULL, WIFI_RESET_BUTTON_TASK_PRIORITY, NULL, WIFI_RESET_BUTTON_TASK_CORE_ID);

	// Install gpio isr service
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

	// Attach the interrupt service routine
	gpio_isr_handler_add(WIFI_RESET_BUTTON, wifi_reset_button_isr_handler, NULL);
}

