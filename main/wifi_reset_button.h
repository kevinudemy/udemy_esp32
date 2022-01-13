/*
 * wifi_reset_button.h
 *
 *  Created on: Nov 1, 2021
 *      Author: kjagu
 */

#ifndef MAIN_WIFI_RESET_BUTTON_H_
#define MAIN_WIFI_RESET_BUTTON_H_

// Default interrupt flag
#define ESP_INTR_FLAG_DEFAULT	0

// Wifi reset button is the BOOT button on the DevKit
#define WIFI_RESET_BUTTON		0

/**
 * Configures Wifi reset button and interrupt configuration
 */
void wifi_reset_button_config(void);

#endif /* MAIN_WIFI_RESET_BUTTON_H_ */
