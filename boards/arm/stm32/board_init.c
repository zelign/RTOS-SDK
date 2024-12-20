/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "board_init.h"
#include "initcall.h"
#include "system.h"
static unsigned int inline get_sp_reg(void)
{
    unsigned int lr;
    __asm volatile (
        "mov %0, sp"
        : "=r" (lr)
        :
        :
    );
    return lr;
}
void app_init(void)
{
    initcall_app_init();
	printf("App initial!\n");
}



