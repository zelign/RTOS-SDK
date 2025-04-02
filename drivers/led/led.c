/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "led.h"
#include "initcall.h"

void led_init(unsigned int led, enum gpio_set original_state)
{
	switch (led) {
	case LED_RED: {
		gpio_init(LED_RED_PORT, LED_RED_PIN, OUTPUT, PUSH, GPIO_2MHZ,
			  UP_PULL, original_state);
		break;
	}
	case LED_GREEN: {
		gpio_init(LED_GREEN_PORT, LED_GREEN_PIN, OUTPUT, PUSH,
			  GPIO_2MHZ, UP_PULL, original_state);
		break;
	}
	default:
		break;
	}
}

void led_light(unsigned int led)
{
	switch (led) {
	case LED_RED:
		gpio_pin_cfg(LED_RED_PORT, LED_RED_PIN, RESET);
		break;
	case LED_GREEN:
		gpio_pin_cfg(LED_GREEN_PORT, LED_GREEN_PIN, RESET);
		break;
	default:
		break;
	}
}

void led_off(unsigned int led)
{
	switch (led) {
	case LED_RED:
		gpio_pin_cfg(LED_RED_PORT, LED_RED_PIN, SET);
		break;
	case LED_GREEN:
		gpio_pin_cfg(LED_GREEN_PORT, LED_GREEN_PIN, SET);
		break;
	default:
		break;
	}
}
