/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __MY_SYSTEM_H__
#define __MY_SYSTEM_H__

#include "tolower.h"
#include "atoi.h"
#include "isalpha.h"
#include "islower.h"
#include "isupper.h"
#include "strcmp.h"
#include "strncmp.h"
#include "strcat.h"
#include "strcpy.h"
#include "strlen.h"
#include "strncat.h"
#include "strncpy.h"
#include "isspace.h"
#include "isdigit.h"
#include "strtoul.h"

#include <stdio.h>

#include <stdlib.h>
#include <stdint.h>
#include "autoconfig.h"

#ifdef CONFIG_NANO_PRINTF
#define NANOPRINTF_VISIBILITY_STATIC
#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"
void _npf_putc(int c, void *priv);
#define nano_printf(format, ...) npf_pprintf(&_npf_putc, NULL, format, ##__VA_ARGS__)
#endif


#define xPortSysTickHandler SysTick_Handler
#define xPortPendSVHandler  PendSV_Handler
#define vPortSVCHandler SVC_Handler

#ifndef SIZE_MAX
#define SIZE_MAX 0xFFFFFFFF
#endif

#ifndef REG32
#define REG32(x)    *(volatile unsigned int *)(x)
#endif

#define NULL ((void *)0)

typedef enum _bool {
    ENABLE = 1,
    DISABLE = 0,
}bool;

enum {
    FALSE = 0,
    TRUE,
};

#define write32(reg, val) \
    do { \
        *(reg) &= val; \
    } while(0);

char *gets_usart1(void);
char getc_usart1_loop (bool *flag);
void putc_usart1(char c);
void puts_usart1(char *s);
void *pvPortMalloc(size_t xWantedSize);
void vPortFree(void * pv);
void *pvPortCalloc(size_t xNum,size_t xSize);

void get_privilege_level(void);
void set_privilege_level(bool level);
void set_sp_level(bool mode);
void get_sp_mode(void);

void put32_usart1(int i);
unsigned int get_sp(bool sp);
unsigned char get_exception_num(void);
unsigned int get_lr_reg(void);
void boot_init(void);
void exit_error(void);
void app_init(void);

bool is_flash_init(void);
void cli_console_init(void);

/* define for memset */
// extern int memset(void *s, int c, unsigned int n);

#define MEMSET_SUCCESS 0
#define MEMSET_COUNT_ERROR 1
#define MEMSET_DST_ERROR 2

/* define for memcpy */
// extern int memcpy(void *dst, void *src, unsigned int n);

#define MEMCPY_SUCCESS 0
#define MEMCPY_COUNT_ERROR 1
#define MEMCPY_DST_ERROR 2
#define MEMCPY_SRC_ERROR 3

/* define for memcmp */
// extern int memcmp(void *dst, void *src, unsigned int n);
#define MEMCMP_EQUAL 0
#define MEMCMP_NOT_EQUAL 1
#define MEMCMP_FAIL 2

#endif
