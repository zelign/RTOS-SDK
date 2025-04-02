/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "initcall.h"
#include "system.h"

void boot_init(void)
{
	initcall_boot_init();
	puts_usart1("Boot initial is Successful");
}

void exit_error()
{
	puts_usart1("exit_error");
	while (1)
		;
}
