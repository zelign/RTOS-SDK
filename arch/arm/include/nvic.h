/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __NVIC_H__
#define __NVIC_H__

#include "board_init.h"
#include "system.h"

#ifdef STM32_F407ZGT6
#include "cortex_M4.h"
#endif

#define NVIC_ENABLE(x) \
    do {   \
        *((volatile unsigned int *)(ISER_BASE_ADDR + (((x) / 32) * 4))) |= \
                                            (0x1 << ((x) % 32)); \
    } while(0);

#define NVIC_DISABLE(x) \
    do {   \
        *((volatile unsigned int *)(ICER_BASE_ADDR + (((x) / 32) * 4))) &=  \
                                            ~(0x1 << ((x) % 32)); \
    } while(0);

void INT_disable (enum EXC_NUM exc_num);

void set_pending_state (enum EXC_NUM exc_num);

void clear_pending_state (enum EXC_NUM exc_num);

unsigned char get_pending_state (enum EXC_NUM exc_num);

unsigned char get_active_state (enum EXC_NUM exc_num);

void set_priority(enum priority_group prio_g,
                  unsigned char empt_prio,
                  unsigned char sub_priority,
                  enum EXC_NUM exc_num);

void set_priority_group(enum priority_group pri);

void NVIC_init(enum EXC_NUM exc_num,
               enum priority_group prio_grp,
               unsigned char empt_prio,
               unsigned char sub_prio);

void systick_init(unsigned int reload_val, bool clk_src, bool exp_enable, unsigned char prio);

unsigned int read_current_count(unsigned int *cnt);

void set_systick_reload_val(unsigned int val);

unsigned char read_count_flag(void);

void systick_delay_ms (unsigned int ms);

void systick_delay_us (unsigned int us);

#endif

