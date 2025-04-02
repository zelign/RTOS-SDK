/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __BOARD_CFG_H__
#define __BOARD_CFG_H__

#include "system.h"

extern unsigned char _sysclk_frq;
extern unsigned char _ahb1_frq;
extern unsigned char _apb1_frq;
extern unsigned char _apb2_frq;

#define STM32_F407ZGT6

#define MODER 0x0
#define OTYPER 0x04
#define OSPEEDER 0x08
#define PUPDR 0x0c
#define IDR 0x10
#define ODR 0x14
#define BSRR 0x18
#define LCKR 0x1C
#define AFRL 0x20
#define AFRH 0x24

enum gpio_reg {
	GPIOA = 0x0,
	GPIOB = 0x00000400,
	GPIOC = 0x00000800,
	GPIOD = 0x00000c00,
	GPIOE = 0x00001000,
	GPIOF = 0x00001400,
	GPIOG = 0x00001800,
	GPIOH = 0x00001c00,
	GPIOI = 0x00002000,
};

enum gpio_pin {
	GPIO_PIN_0 = 0,
	GPIO_PIN_1,
	GPIO_PIN_2,
	GPIO_PIN_3,
	GPIO_PIN_4,
	GPIO_PIN_5,
	GPIO_PIN_6,
	GPIO_PIN_7,
	GPIO_PIN_8,
	GPIO_PIN_9,
	GPIO_PIN_10,
	GPIO_PIN_11,
	GPIO_PIN_12,
	GPIO_PIN_13,
	GPIO_PIN_14,
	GPIO_PIN_15,
};

enum gpio_moder {
	INPUT = 0,
	OUTPUT = 1,
	REUSE,
	SIMUL,
};

enum gpio_otype {
	PUSH = 0,
	OPEN,
};

enum gpio_speed {
	GPIO_2MHZ = 0,
	GPIO_25MHZ,
	GPIO_50MHZ,
	GPIO_100MHZ,
};

enum gpio_uppull {
	NONE_PULL = 0,
	UP_PULL,
	DOWN_PULL,
	REVERSE_PULL,
};

enum gpio_set {
	RESET = 0,
	SET,
};

enum gpio_afr {
	AF0 = 0,
	AF1,
	AF2,
	AF3,
	AF4,
	AF5,
	AF6,
	AF7,
	AF8,
	AF9,
	AF10,
	AF11,
	AF12,
	AF13,
	AF14,
	AF15,
};

/* Define periphral base address */
#define PERIPHRAL_BASE_ADDR (uint32_t)0x40000000

/* Define APB1 bus address */
#define APB1_BASE_ADDR (PERIPHRAL_BASE_ADDR)

/* Define APB2 bus address */
#define APB2_BASE_ADDR (PERIPHRAL_BASE_ADDR + 0x10000)

/* Define AHB1 bus address */
#define AHB1_BASE_ADDR (PERIPHRAL_BASE_ADDR + 0x20000)

/* Define AHB2 bus address */
#define AHB2_BASE_ADDR (PERIPHRAL_BASE_ADDR + 0x10000000)

/* Define AHB3 bus address */
#define AHB3_BASE_ADDR (PERIPHRAL_BASE_ADDR + 0x20000000)

/**************************************GPIO Registers**************************************/
/* Define GPIO base address */
#define GPIO_BASE_ADDR (AHB1_BASE_ADDR)

/* Define GPIO_A base address */
#define GPIO_A_BASE_ADDR (GPIO_BASE_ADDR + GPIOA)

/************** Define GPIO_A registers **************/
/* port mode register */
#define GPIO_A_MODER *(volatile uint32_t *)(GPIO_A_BASE_ADDR + MODER)
/* port output type register */
#define GPIO_A_OTYPER *(volatile uint32_t *)(GPIO_A_BASE_ADDR + OTYPER)
/* port output speed register */
#define GPIO_A_OSPEEDR *(volatile uint32_t *)(GPIO_A_BASE_ADDR + OSPEEDER)
/* port pull-up/pull-down register */
#define GPIO_A_PUPDR *(volatile uint32_t *)(GPIO_A_BASE_ADDR + PUPDR)
/* port input data register */
#define GPIO_A_IDR *(volatile uint32_t *)(GPIO_A_BASE_ADDR + IDR)
/* port output data register */
#define GPIO_A_ODR *(volatile uint32_t *)(GPIO_A_BASE_ADDR + ODR)
/* port bit set/reset register */
#define GPIO_A_BSRR *(volatile uint32_t *)(GPIO_A_BASE_ADDR + BSRR)
/* port configuration lock register */
#define GPIO_A_LCKR *(volatile uint32_t *)(GPIO_A_BASE_ADDR + LCKR)
/* port alternate function low register */
#define GPIO_A_AFRL *(volatile uint32_t *)(GPIO_A_BASE_ADDR + AFRL)
/* port alternate function high register */
#define GPIO_A_AFRH *(volatile uint32_t *)(GPIO_A_BASE_ADDR + AFRH)

#define GPIO_B_BASE_ADDR (GPIO_BASE_ADDR + (unsigned int)GPIOB)
/************** Define GPIO_B registers **************/
/* port mode register */
#define GPIO_B_MODER *(volatile uint32_t *)(GPIO_B_BASE_ADDR + MODER)
/* port output type register */
#define GPIO_B_OTYPER *(volatile uint32_t *)(GPIO_B_BASE_ADDR + OTYPER)
/* port output speed register */
#define GPIO_B_OSPEEDR *(volatile uint32_t *)(GPIO_B_BASE_ADDR + OSPEEDER)
/* port pull-up/pull-down register */
#define GPIO_B_PUPDR *(volatile uint32_t *)(GPIO_B_BASE_ADDR + PUPDR)
/* port input data register */
#define GPIO_B_IDR *(volatile uint32_t *)(GPIO_B_BASE_ADDR + IDR)
/* port output data register */
#define GPIO_B_ODR *(volatile uint32_t *)(GPIO_B_BASE_ADDR + ODR)
/* port bit set/reset register */
#define GPIO_B_BSRR *(volatile uint32_t *)(GPIO_B_BASE_ADDR + BSRR)
/* port configuration lock register */
#define GPIO_B_LCKR *(volatile uint32_t *)(GPIO_B_BASE_ADDR + LCKR)
/* port alternate function low register */
#define GPIO_B_AFRL *(volatile uint32_t *)(GPIO_B_BASE_ADDR + AFRL)
/* port alternate function high register */
#define GPIO_B_AFRH *(volatile uint32_t *)(GPIO_B_BASE_ADDR + AFRH)

/* Define GPIO_E base address */
#define GPIO_E_BASE_ADDR (GPIO_BASE_ADDR + GPIOE)

/************** Define GPIO_F registers **************/
/* port mode register */
#define GPIO_E_MODER *(volatile uint32_t *)(GPIO_E_BASE_ADDR + MODER)
/* port output type register */
#define GPIO_E_OTYPER *(volatile uint32_t *)(GPIO_E_BASE_ADDR + OTYPER)
/* port output speed register */
#define GPIO_E_OSPEEDR *(volatile uint32_t *)(GPIO_E_BASE_ADDR + OSPEEDER)
/* port pull-up/pull-down register */
#define GPIO_E_PUPDR *(volatile uint32_t *)(GPIO_E_BASE_ADDR + PUPDR)
/* port input data register */
#define GPIO_E_IDR *(volatile uint32_t *)(GPIO_E_BASE_ADDR + IDR)
/* port output data register */
#define GPIO_E_ODR *(volatile uint32_t *)(GPIO_E_BASE_ADDR + ODR)
/* port bit set/reset register */
#define GPIO_E_BSRR *(volatile uint32_t *)(GPIO_E_BASE_ADDR + BSRR)
/* port configuration lock register */
#define GPIO_E_LCKR *(volatile uint32_t *)(GPIO_E_BASE_ADDR + LCKR)
/* port alternate function low register */
#define GPIO_E_AFRL *(volatile uint32_t *)(GPIO_E_BASE_ADDR + AFRL)
/* port alternate function high register */
#define GPIO_E_AFRH *(volatile uint32_t *)(GPIO_E_BASE_ADDR + AFRH)

/* Define GPIO_F base address */
#define GPIO_F_BASE_ADDR (GPIO_BASE_ADDR + 0x1400)

/************** Define GPIO_F registers **************/
/* port mode register */
#define GPIO_F_MODER *(volatile uint32_t *)(GPIO_F_BASE_ADDR + MODER)
/* port output type register */
#define GPIO_F_OTYPER *(volatile uint32_t *)(GPIO_F_BASE_ADDR + OTYPER)
/* port output speed register */
#define GPIO_F_OSPEEDR *(volatile uint32_t *)(GPIO_F_BASE_ADDR + OSPEEDER)
/* port pull-up/pull-down register */
#define GPIO_F_PUPDR *(volatile uint32_t *)(GPIO_F_BASE_ADDR + PUPDR)
/* port input data register */
#define GPIO_F_IDR *(volatile uint32_t *)(GPIO_F_BASE_ADDR + IDR)
/* port output data register */
#define GPIO_F_ODR *(volatile uint32_t *)(GPIO_F_BASE_ADDR + ODR)
/* port bit set/reset register */
#define GPIO_F_BSRR *(volatile uint32_t *)(GPIO_F_BASE_ADDR + BSRR)
/* port configuration lock register */
#define GPIO_F_LCKR *(volatile uint32_t *)(GPIO_F_BASE_ADDR + LCKR)
/* port alternate function low register */
#define GPIO_F_AFRL *(volatile uint32_t *)(GPIO_F_BASE_ADDR + AFRL)
/* port alternate function high register */
#define GPIO_F_AFRH *(volatile uint32_t *)(GPIO_F_BASE_ADDR + AFRH)

/**************************************CLOCK Registers**************************************/
/* Define clock registers */
#define CLOCK_BASE_ADDR (AHB1_BASE_ADDR + 0x3800)

/* RCC clock control register */
#define RCC_CR *((volatile uint32_t *)(CLOCK_BASE_ADDR + 0x0))

/* RCC PLL configuration register */
#define RCC_PLLCFGR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x04)

/* RCC clock configuration register */
#define RCC_CFGR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x08)

/* RCC clock interrupt register */
#define RCC_CIR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x0c)

/* RCC AHB1 periphrial reset register */
#define RCC_AHB1RSTR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x10)

/* RCC AHB2 periphrial reset register */
#define RCC_AHB2RSTR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x14)

/* RCC AHB3 periphrial reset register */
#define RCC_AHB3RSTR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x18)

/* RCC APB1 periphrial reset register */
#define RCC_APB1RSTR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x20)

/* RCC APB2 periphrial reset register */
#define RCC_APB2RSTR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x24)

/* Define RCC AHB1 peripheral clock enable reigster */
#define RCC_AHB1ENR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x30)

/* Define RCC AHB2 peripheral clock enable reigster */
#define RCC_AHB2ENR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x34)

/* Define RCC AHB3 peripheral clock enable reigster */
#define RCC_AHB3ENR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x38)

/* Define RCC APB1 peripheral clock enable reigster */
#define RCC_APB1ENR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x40)

/* Define RCC APB2 peripheral clock enable reigster */
#define RCC_APB2ENR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x44)

/* RCC Backup domain control register */
#define RCC_BDCR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x70)

/* RCC clock control & status register */
#define RCC_CSR *(volatile uint32_t *)(CLOCK_BASE_ADDR + 0x74)

enum clock_src {
	HSI = 0,
	HSE,
	PLL,
	NO_CLK_SRC,
};

enum clock_hclk_presc {
	HPRE_NO_FRE = 0x0000,
	HPRE_2 = 8,
	HPRE_4 = 9,
	HPRE_8,
	HPRE_16,
	HPRE_64,
	HPRE_128,
	HPRE_256,
	HPRE_512,
};

enum clock_pclk_presc {
	PPRE_0 = 0,
	PPRE_2 = 4,
	PPRE_4,
	PPRE_8,
	PPRE_16,
};

enum clock_pll_presc {
	PLLQ_ERR0 = 0,
	PLLQ_ERR1 = 1,
	PLLQ_2,
	PLLQ_3,
	PLLQ_4,
	PLLQ_5,
	PLLQ_6,
	PLLQ_7,
	PLLQ_8,
	PLLQ_9,
	PLLQ_10,
	PLLQ_11,
	PLLQ_12,
	PLLQ_13,
	PLLQ_14,
	PLLQ_15,
	PLLP_2 = 0,
	PLLP_4 = 1,
	PLLP_6 = 2,
	PLLP_8 = 3,
	PLLN_ERR0 = 0,
	PLLN_ERR1 = 1,
	PLLN_336 = 336,
	PLLM_ERR0 = 0,
	PLLM_ERR1 = 1,
	PLLM_2 = 2,
	PLLM_3 = 3,
	PLLM_8 = 8,
	PLLM_16 = 16,
	PLLM_25 = 25,
};

enum clock_osc {
	HSI_OSC = 0,
	HSE_OSC,
	LSI_OSC,
	LSE_OSC,
};

/**************************************Flash Interface Registers**************************************/
#define FLASH_BASE_ADDR (AHB1_BASE_ADDR + 0x3c00)

/* Flash access control register */
#define FLASH_ACR *(volatile uint32_t *)(FLASH_BASE_ADDR + 0x0)
enum flash_access_f {
	LATENCY_PERIOD_0 = 0,
	LATENCY_PERIOD_1,
	LATENCY_PERIOD_2,
	LATENCY_PERIOD_3,
	LATENCY_PERIOD_4,
	LATENCY_PERIOD_5,
	LATENCY_PERIOD_6,
	LATENCY_PERIOD_7,
};

/********************************SYSCFG*********************************/
#define SYSCFG_BASE_ADDR (APB2_BASE_ADDR + 0x3800)

/* SYSCFG external interrupt configuration register 1 */
#define SYSCFG_EXTICR (SYSCFG_BASE_ADDR + 0x08)
#define SYSCFG_EXTICR1 (SYSCFG_BASE_ADDR + 0x08)
#define SYSCFG_EXTICR2 (SYSCFG_BASE_ADDR + 0x0C)
#define SYSCFG_EXTICR3 (SYSCFG_BASE_ADDR + 0x10)
#define SYSCFG_EXTICR4 (SYSCFG_BASE_ADDR + 0x14)

/*********************************EXTI**********************************/
#define EXTI_BASE_ADDR (APB2_BASE_ADDR + 0x3C00)

/* define the interrupt mask register */
#define EXTI_IMR (EXTI_BASE_ADDR + 0x0)

/* event mask register */
#define EXTI_EMR (EXTI_BASE_ADDR + 0x4)

/* rising trigger selection register */
#define EXTI_RTSR (EXTI_BASE_ADDR + 0x8)

/* falling trigger selection register */
#define EXTI_FTSR (EXTI_BASE_ADDR + 0x0C)

/* software interrupt event register */
#define EXTI_SWIER (EXTI_BASE_ADDR + 0x10)

/* pending register */
#define EXTI_PR (EXTI_BASE_ADDR + 0x14)

/* Universal Synchronous Asynchronous Receiver/Transmiter(USART)  */
#define USART1_BASE_ADDR (APB2_BASE_ADDR + 0x1000)

/* status register */
#define USART1_SR *(volatile unsigned int *)(USART1_BASE_ADDR + 0x0)

/* data register */
#define USART1_DR *(volatile unsigned int *)(USART1_BASE_ADDR + 0x04)

/* boud rate register */
#define USART1_BRR *(volatile unsigned int *)(USART1_BASE_ADDR + 0x08)

/* control register 1 */
#define USART1_CR1 *(volatile unsigned int *)(USART1_BASE_ADDR + 0x0C)

/* control register 2 */
#define USART1_CR2 *(volatile unsigned int *)(USART1_BASE_ADDR + 0x10)

/* control register 3 */
#define USART1_CR3 *(volatile unsigned int *)(USART1_BASE_ADDR + 0x14)

enum oversamping_mode { over_16 = 0, over_8 };

enum word_length {
	word_length_8 = 0,
	word_length_9,
};

enum parity_varify {
	disable_parity = 0,
	enable_parity,
};

enum usart_interrupt_control {
	IDLE_IE = 4,
	RXNE_IE,
	TC_IE,
	TXE_IE,
	PE_IE,
};

enum usart_base_control {
	RE = 2,
	TE,
	PCE = 10,
	UE = 13,
};

enum usart_stop_bit {
	STOP_1 = 0,
	STOP_0_5,
	STOP_2,
	STOP_1_5,
};

/********************************SPI*********************************/
//SPI1
#define SPI1_BASE_ADDR (APB2_BASE_ADDR + 0x3000)
#define SPI2_BASE_ADDR (APB1_BASE_ADDR + 0x3800)
#define SPI3_BASE_ADDR (APB1_BASE_ADDR + 0x3C00)
#define SPI4_BASE_ADDR (APB2_BASE_ADDR + 0x3400)

enum spi_dev {
	SPI_1 = SPI1_BASE_ADDR,
	SPI_2 = SPI2_BASE_ADDR,
	SPI_3 = SPI3_BASE_ADDR,
	SPI_4 = SPI4_BASE_ADDR,
};

//spi control register
#define SPI_CR1 0x0
#define SPI_CR2 0x4

//spi status register
#define SPI_SR 0x8

//spi data register
#define SPI_DR 0xC

//spi crc polynomial register
#define SPI_CRCPR 0x10

//spi RX CRC register
#define SPI_RXCRC 0x14

//spi TX CRC register
#define SPI_TXCRC 0x18

//spi iis configuration register
#define SPI_I2SCFG 0x1C

//spi iis prescaler register
#define SPI_I2SPR 0x20

enum trans_direct_type {
	double_line_single_direct = 0,
	single_line_double_direct = 1,
};

enum data_frame_format_e {
	send_rev_select_8_bit = 0,
	send_rev_select_16_bit,
};

enum software_slave_management_e {
	disable_software_management = 0,
	enable_software_management,
};

enum rev_mode {
	duplex = 0,
	rx_only,
};

enum lsb_first_e {
	first_msb = 0,
	first_lsb,
};

enum baud_rate_control_e {
	pclk_divide_2 = 0,
	pclk_divide_4,
	pclk_divide_8,
	pclk_divide_16,
	pclk_divide_32,
	pclk_divide_64,
	pclk_divide_128,
	pclk_divide_256,
};

enum master_selection_e {
	slave_mode = 0,
	master_mode,
};

enum clock_polarity_phase_e {
	free_lower_first_edge_sample = 0,
	free_lower_second_edge_sample,
	free_higher_first_edge_sample,
	free_higher_second_dege_sample,
};

enum frame_format_e {
	motorola_mode = 0,
	ti_mode = 1,
};

#endif /* __BOARD_CFG_H__ */
