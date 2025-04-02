/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __CORTEX_M4_H__
#define __CORTEX_M4_H__

#include "board_init.h"
#ifdef STM32_F407ZGT6
/********************* Define the NVIC register ***********************/
#define PRIORITY_MAX_BIT_NUM 4
/**
 * The NVIC register is stored in System Control Space where start address is 0xE000E000
*/
#define SCS_BASE_ADDR 0xE000E000

/* define the systick registers */
#define SYST_BASE_ADDR 0xE000E010
#define SYST_CSR (SYST_BASE_ADDR + 0x0) //control and status register
#define SYST_RVR (SYST_BASE_ADDR + 0x04) //reloader value register
#define SYST_CVR (SYST_BASE_ADDR + 0x08) //current value register
#define SYST_CALIB (SYST_BASE_ADDR + 0x0C) //calibration value register

#define DISABLE_SYST (*(volatile unsigned int *)SYST_CSR &= ~0x1)

/* define the interrupt registers */
#define ISER_BASE_ADDR 0xE000E100
#define ICER_BASE_ADDR 0xE000E180
#define ISPR_BASE_ADDR 0xE000E200
#define ICPR_BASE_ADDR 0xE000E280
#define IABR_BASE_ADDR 0xE000E300
#define IP_BASE_ADDR 0xE000E400

/* define the system handler priority register */
#define SHPR_1 (SCS_BASE_ADDR + 0x18)
#define SHPR_BASE_ADDR (SHPR_1)
#define SHPR_2 (SCS_BASE_ADDR + 0x1C)
#define SHPR_3 (SCS_BASE_ADDR + 0x20)

/* define SCB register */
#define SCB_BASE_ADDR 0xE000ED00
#define CPUID (SCB_BASE_ADDR + 0x0)
#define ICSR (SCB_BASE_ADDR + 0x4)
#define VTOR (SCB_BASE_ADDR + 0x08)
#define AIRCR (SCB_BASE_ADDR + 0x0C)
#define SCR (SCB_BASE_ADDR + 0x10)
#define CCR (SCB_BASE_ADDR + 0x14)

#endif

//define the exception numbers
enum EXC_NUM {
	SYS_RST = 1, //reset
	SYS_NMI = 2, //non-markable interupt
	SYS_HWF = 3, //hardware fault
	SYS_MMF = 4, //MemManage fault
	SYS_BSF = 5, //bus fault
	SYS_UGF = 6, //usage falut
	SYS_SVC = 11, //pengd svc
	SYS_DBM = 12, //debug monitor
	SYS_PSV = 14, //pend sv
	SYS_STK = 15, //systick
#ifdef STM32_F407ZGT6
	INT_WDG = 16, //watch dog
	INT_PVD = 17,
	INT_STP = 18, //stamp
	INT_RTC_WKP = 19, //rtc weakup
	INT_FLS = 20, //flash global interrupt
	INT_RCC = 21, //RCC global interrupt
	INT_EXT0 = 22, //external 0 line
	INT_EXT1 = 23, //external 1 line
	INT_EXT2 = 24, //external 2 line
	INT_EXT3 = 25, //external 3 line
	INT_EXT4 = 26, //external 4 line
	INT_DMA1_STM0 = 27,
	INT_DMA1_STM1 = 28,
	INT_DMA1_STM2 = 29,
	INT_DMA1_STM3 = 30,
	INT_DMA1_STM4 = 31,
	INT_DMA1_STM5 = 32,
	INT_DMA1_STM6 = 33,
	INT_ADC = 34,
	INT_CAN1_TX = 35,
	INT_CAN1_RX0 = 36,
	INT_CAN1_RX1 = 37,
	INT_CAN1_SCE = 38,
	INT_EXT5_9 = 39,
	INT_TIM1_BRK_TIM9 = 40,
	INT_TIM1_UP_TIM10 = 41,
	INT_TRG_COM_TIM11 = 42,
	INT_TIM1_CC = 43,
	INT_TIM2 = 44,
	INT_TIM3 = 45,
	INT_TIM4 = 46,
	INT_IIC1_EV = 47,
	INT_IIC1_ER = 48,
	INT_IIC2_EV = 49,
	INT_IIC2_ER = 50,
	INT_SPI1 = 51,
	INT_SPI2 = 52,
	INT_USART1 = 53,
	INT_USART2 = 54,
	INT_USART3 = 55,
	INT_EXT10_15 = 56,
	INT_RTC_ALM = 57,
#endif
};

/* The cortex-M4 support 4-bit priority */
enum priority_group {
	EMPT_4_SUB_0 = 0x03,
	EMPT_3_SUB_1 = 0x04,
	EMPT_2_SUB_2 = 0x05,
	EMPT_1_SUB_3 = 0x06,
	EMPT_0_SUB_4 = 0x07,
};

#endif
