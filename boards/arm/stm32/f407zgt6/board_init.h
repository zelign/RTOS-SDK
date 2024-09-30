/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __BOARD_INIT_H__
#define __BOARD_INIT_H__

#include "board_cfg.h"
#include "nvic.h"


/**
 * @func: Select the oscillator for clock.
 * @para: osc the clock oscillator.
 *
 */
void clock_osc_cfg(enum clock_osc osc);

#endif
