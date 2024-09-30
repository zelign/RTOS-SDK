/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __USART_H__
#define __USART_H__

#include "gpio.h"
#include "board_init.h"
#include "system.h"

#define STM32_F407_PZ_CORE_BOARD

#define USART1_ENABLE_CLK   RCC_APB2ENR |= (0x1 << 4)

static inline void base_functionality_ctl(enum usart_base_control ubc, bool flag)
{
    USART1_CR1 &= ~(0x1 << ubc);
    USART1_CR1 |= (flag << ubc);
}

static inline void interrupt_ctl(enum usart_interrupt_control ie, bool flag)
{
    USART1_CR1 &= ~(0x1 << ie);
    USART1_CR1 |= (flag << ie);
}

#define USART1_ENABLE_PARITY   base_functionality_ctl(PCE, ENABLE)
#define USART1_DISABLE_PARITY   base_functionality_ctl(PCE, DISABLE)

#define USART1_ENABLE_RE   base_functionality_ctl(RE, ENABLE)
#define USART1_DISABLE_RE  base_functionality_ctl(RE, DISABLE)

#define USART1_ENABLE_TE   base_functionality_ctl(TE, ENABLE)
#define USART1_DISABLE_TE  base_functionality_ctl(TE, DISABLE)

#define USART1_ENABLE_UE  base_functionality_ctl(UE, ENABLE)
#define USART1_DISABLE_UE  base_functionality_ctl(UE, DISABLE)

#define USART1_DISABLE_TXE_IE interrupt_ctl(TXE_IE,DISABLE)
#define USART1_ENABLE_TXE_IE interrupt_ctl(TXE_IE,ENABLE)

#define USART1_ENABLE_RXNE_IE interrupt_ctl(RXNE_IE,ENABLE)
#define USART1_DISABLE_RXNE_IE interrupt_ctl(RXNE_IE,DISABLE)

#define USART1_ENABLE_IDLE_IE interrupt_ctl(IDLE_IE,ENABLE)
#define USART1_DISABLE_IDLE_IE interrupt_ctl(IDLE_IE,DISABLE)

#define USART1_ENABLE_PE_IE interrupt_ctl(PE_IE,ENABLE)
#define USART1_DISABLE_PE_IE interrupt_ctl(PE_IE,DISABLE)

#define USART1_ENABLE_TC_IE interrupt_ctl(TC_IE,ENABLE)
#define USART1_DISABLE_TC_IE interrupt_ctl(TC_IE,DISABLE)


void usart1_init();


#endif // __USART_H__
