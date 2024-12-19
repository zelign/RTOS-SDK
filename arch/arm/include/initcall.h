/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __INITCALL_H__
#define __INITCALL_H__

typedef void (*initcall_fp)(void);

#define _INIT_CALL_MODLE(fuc, level) \
static volatile initcall_fp \
_initcall_##fuc \
__attribute__((__used__)) \
__attribute__((section(".init_call."level))) = (initcall_fp)fuc

#define BOOT_INIT_1(x) _INIT_CALL_MODLE(x, "boot_1")
#define BOOT_INIT_2(x) _INIT_CALL_MODLE(x, "boot_2")
#define BOOT_INIT_3(x) _INIT_CALL_MODLE(x, "boot_3")

#define APP_INIT_1(x) _INIT_CALL_MODLE(x, "application_1")
#define APP_INIT_2(x) _INIT_CALL_MODLE(x, "application_2")
#define APP_INIT_3(x) _INIT_CALL_MODLE(x, "application_3")


extern initcall_fp _boot_init_start[];
extern initcall_fp _boot_init_end[];
extern initcall_fp _app_init_start[];
extern initcall_fp _app_init_end[];

void initcall_app_init(void);
void initcall_boot_init(void);

#endif
