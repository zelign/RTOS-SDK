/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <stdint.h>

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
#if 0
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned int size_t;


typedef char    int8_t;
typedef short   int16_t;
typedef int     int32_t;
typedef long long int64_t;
#endif

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

void sm_printf(const char *fmt, ...);
void cli_console_init(void);

#endif
