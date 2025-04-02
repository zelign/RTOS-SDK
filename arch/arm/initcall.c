/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "initcall.h"
#include "system.h"

void initcall_app_init(void)
{
	initcall_fp *init_func = NULL;

	for (init_func = _app_init_start; init_func < _app_init_end;
	     init_func++) {
		(*init_func)();
	}
}

void initcall_boot_init(void)
{
	initcall_fp *init_func = NULL;

	for (init_func = _boot_init_start; init_func < _boot_init_end;
	     init_func++) {
		(*init_func)();
	}
}
