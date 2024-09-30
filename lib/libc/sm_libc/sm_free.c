/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "sm_free.h"
#include <FreeRTOS.h>

void free(void *ptr)
{
	vPortFree(ptr);
}
