#ifndef __CLOCK_H__
#define __CLOCK_H__

/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "board_init.h"

#define HSE_CLK_SRC 8
void clock_init(void);
void flash_access_frq(enum flash_access_f faq);

#endif // ! __CLOCK_H__
