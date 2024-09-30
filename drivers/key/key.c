/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "key.h"
#include "system.h"
#include "initcall.h"
#include "exti.h"

void key_init(unsigned int key)
{
#ifdef STM32_F407_PZ_CORE_BOARD
    switch(key) {
        case KEY_UP_PIN_UP:
        {
            gpio_init(KEY_PORT_UP,key,INPUT,-1,-1,DOWN_PULL,-1);
            return;
        }
        case KEY_DOWN_PIN_0:
        case KEY_DOWN_PIN_1:
        {
            gpio_init(KEY_PORT_DOWN,key,INPUT,-1,-1,UP_PULL,-1);
            return;
        }
        default:
        return;
    }
#endif
}

unsigned char read_key(unsigned int key)
{
#ifdef STM32_F407_PZ_CORE_BOARD
    switch (key) {
        case KEY_UP_PIN_UP:
        {
            return (((GPIO_A_IDR >> key ) & 0x1 ) ? 1 : 0);
        }
        case KEY_DOWN_PIN_0:
        case KEY_DOWN_PIN_1:
        {
            return (((GPIO_E_IDR >> key) & 0x1) ? 1 : 0);
        }

        default:
        return 0;
    }
#endif
}

