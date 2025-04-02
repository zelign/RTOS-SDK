/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __KEY_H__
#define __KEY_H__
#include "gpio.h"

#define STM32_F407_PZ_CORE_BOARD
#define KEY_ON 1U
#define KEY_OFF 0U

#ifdef STM32_F407_PZ_CORE_BOARD
#define KEY_PORT_DOWN GPIOE
#define KEY_DOWN_PIN_0 GPIO_PIN_3
#define KEY_DOWN_PIN_1 GPIO_PIN_4

#define KEY_PORT_UP GPIOA
#define KEY_UP_PIN_UP GPIO_PIN_0
#else
#define KEY_1
#define KEY_0
#define KEY_UP
#endif

void key_init(unsigned int key);
unsigned char read_key(unsigned int key);

#endif
