/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "board_init.h"

void gpio_init(enum gpio_reg gpio, enum gpio_pin pin, enum gpio_moder mod,
	       enum gpio_otype typ, enum gpio_speed spd, enum gpio_uppull pul,
	       enum gpio_set gset);

void gpio_port_set(enum gpio_reg GPIO, enum gpio_pin pin, enum gpio_moder moder,
		   enum gpio_otype oty, enum gpio_speed speed,
		   enum gpio_uppull pull);
void gpio_pin_cfg(enum gpio_reg GPIO, enum gpio_pin pin, enum gpio_set gset);
void enable_gpio_clock(enum gpio_reg gpio);
void pin_afr(enum gpio_reg gpio, enum gpio_pin pin, enum gpio_afr afr);

#endif
