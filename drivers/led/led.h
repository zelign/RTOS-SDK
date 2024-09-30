/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __LED_H__
#define __LED_H__

#include "gpio.h"

#define STM32_F407_PZ_CORE_BOARD

#ifdef STM32_F407_PZ_CORE_BOARD
#define LED_RED    1U
#define LED_RED_PORT   GPIOF
#define LED_RED_PIN    GPIO_PIN_9
#define LED_GREEN    2U
#define LED_GREEN_PORT   GPIOF
#define LED_GREEN_PIN    GPIO_PIN_10
#else
#define LED_RED
#define LED_GREEN
#endif

void led_init(unsigned int led, enum gpio_set original_state);
void led_light(unsigned int led);
void led_off(unsigned int led);

#endif // !__LED_H__
