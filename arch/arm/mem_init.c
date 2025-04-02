/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */
#include "FreeRTOS.h"
#include "initcall.h"

extern unsigned char _heap_start[];
extern unsigned char _heap_end[];

void mem_init(void)
{
	HeapRegion_t xHeapRegions[] = {
		{ _heap_start, (size_t)(_heap_end - _heap_start) }, { NULL, 0 }
	};

	vPortDefineHeapRegions(xHeapRegions);
}
BOOT_INIT_1(mem_init);