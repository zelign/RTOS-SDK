/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "clock.h"
#include "initcall.h"
#include "../../output/config.h"

/* This definition will be used to identify the clock source */
#define HSE_CLK 1

/* This variable to indicate the system clock frequency, the value
 * will be used by systick delay functin
 */

unsigned char _sysclk_frq = 0;
unsigned char _ahb1_frq;
unsigned char _apb1_frq;
unsigned char _apb2_frq;

/**
 * If you want to use clock, the struct is the entry to configurate the clock according to your 
 * owner requirement.
 */
struct clock_para {
	enum clock_osc osc_src; /* select the oscillator */
	enum clock_src sysclk_src; /* select clock source for system */
	enum clock_src mpll_src; /* main pll's clock source */
	enum clock_pll_presc mpll_q;
	enum clock_pll_presc mpll_p;
	enum clock_pll_presc mpll_n;
	enum clock_pll_presc mpll_m;
    enum clock_hclk_presc hclk;
    enum clock_pclk_presc pclk1;
    enum clock_pclk_presc pclk2;
};

static struct clock_para clk_int;

static void main_pll_cfg (enum clock_pll_presc pllq,
                    enum clock_pll_presc pllp,
                    enum clock_pll_presc plln,
                    enum clock_pll_presc pllm)
{
    RCC_PLLCFGR &= ~(0xf << 24);
    RCC_PLLCFGR |= (pllq << 24);

    RCC_PLLCFGR &= ~(0x3 << 16);
    RCC_PLLCFGR |= (pllp << 16);

    RCC_PLLCFGR &= ~(0x1ff << 6);
    RCC_PLLCFGR |= (plln << 6);

    RCC_PLLCFGR &= ~(0x3f << 0);
    RCC_PLLCFGR |= (pllm << 0);

	/* enable PLL */
    RCC_CR &= ~(1 << 24);
    RCC_CR |= (1 << 24);
    do {
    } while(!((RCC_CR >> 25) & 0x01));
	
}

void select_pll_clk_src (enum clock_src src)
{
	switch (src) {
		case HSI:
		{
            /* select HSI clock source for main PLL */
			RCC_PLLCFGR &= ~(1 << 22);
			break;
		}
		case HSE:
		{
			/* select HSE clock source for main PLL */
			RCC_PLLCFGR &= ~(1 << 22);
			RCC_PLLCFGR |= (1 << 22);
			break;
		}
		default:
			break;
	}
}

/**
 * @func: Select the oscillator for clock.
 * @para: osc the clock oscillator.
 *
 */
void clock_osc_cfg(enum clock_osc osc)
{
    RCC_CR &= ~(0xf << 16);
    switch (osc) {
        case HSI_OSC:
        {
			do {
				/* open HSI OSC */
				RCC_CR |= (1 << 0);
			}while(!(RCC_CR & (1 << 1)));
			break;
        }
        case HSE_OSC:
        {
			do {
				/* open HSE OSC */
				RCC_CR |= (1 << 16);
			}while(!(RCC_CR & (1 << 17)));
			break;
        }
        case LSI_OSC:
        case LSE_OSC:
            break;
        default:
            break;
    }
}

static void switch_sys_clk(enum clock_src src)
{
	switch (src) {
		case HSI:
		{
			break;
		}
		case HSE:
		{
			break;
		}
		case PLL:
		{
			/* select main PLL as the system clock source */
			RCC_CFGR &= ~(0x3 << 0);
			RCC_CFGR |= 0x02;
			do {
			} while(((RCC_CFGR >> 2) & 0x02) != 2);
			break;
		}
		default:
			break;
	}
}

static void hclk_cfg(enum clock_hclk_presc hclk)
{
    RCC_CFGR &= ~(0xf << 4);
    RCC_CFGR |= (hclk << 4);
}

static void pclk1_cfg (enum clock_pclk_presc pclk1)
{
    RCC_CFGR &= ~(0x7 << 10);
    RCC_CFGR |= (pclk1 << 10);
}

static void pclk2_cfg (enum clock_pclk_presc  pclk2)
{
    RCC_CFGR &= ~(0x7 << 13);
    RCC_CFGR |= (pclk2 << 13);
}

void flash_access_frq(enum flash_access_f faq)
{

	FLASH_ACR &= ~(1 << 9);
	FLASH_ACR |= 1 << 9;

	FLASH_ACR &= ~(1 << 10);
	FLASH_ACR |= 1 << 10;

	FLASH_ACR |= ~(1 << 8);
	FLASH_ACR |= 1 << 8;

	FLASH_ACR &= ~(0xFF << 0);
	FLASH_ACR |= faq;
}

/**
 * Initialized the clock according to the follow procedure:
 * 1. select the oscillator.
 * 	# The oscillator is opened complete indicate that the clock source has
 * 	# generate the clock frequency, and then you need guide the frequency 
 * 	# to arrive processor and peripherals.
 * 2. cpnfigurate the frequency factor for system clock(hclk) and peripheral clocks(pclk).
 * 3. configurate the PLL if the system clock source is MAIN PLL.
 *	3.1 pll_presc frequency factor
 *	3.2 enable main pll
 *	3.3 select the clock source for main pll
 * 4. set flash access register (The rate to fetch the code from flash).
 * 5. select the system clock source (open the clock).
 */
void clock_init(void) {

/* If the clock source is high speed external clock */
#ifdef HSE_CLK
	/* configurate initial parameters */
	clk_int.osc_src = HSE_OSC;
	clk_int.sysclk_src = PLL;
	clk_int.mpll_src = HSE;
	clk_int.mpll_q = PLLQ_7;
	clk_int.mpll_p = PLLP_2;
	clk_int.mpll_n = PLLN_336;
	clk_int.mpll_m = PLLM_8;
	clk_int.hclk = HPRE_NO_FRE;
	clk_int.pclk1 = PPRE_4;
	clk_int.pclk2 = PPRE_2;
#elif HSI_CLK
	/* configurate initial parameters */
	clk_int.osc_src = HSI_OSC;
	clk_int.sysclk_src = PLL;
	clk_int.mpll_src = HSI;
	clk_int.mpll_q = PLLQ_7;
	clk_int.mpll_p = PLLP_2;
	clk_int.mpll_n = PLLN_336;
	clk_int.mpll_m = PLLM_16;
	clk_int.hclk = HPRE_NO_FRE;
	clk_int.pclk1 = PPRE_4;
	clk_int.pclk2 = PPRE_2;
#endif

	/* select oscillator */
	clock_osc_cfg(clk_int.osc_src);

	/* configurate system clock frequency */
	hclk_cfg(clk_int.hclk);

	/* configurate APB1(low speed) clock frequency */
	pclk1_cfg(clk_int.pclk1);

	/* configurate APB2 (high speed) clock frequency */
	pclk2_cfg(clk_int.pclk2);

	/* if the system clock source is Main PLL, then set PLL */
	if (clk_int.sysclk_src == PLL) {

		/* select clock source for main PLL */
		select_pll_clk_src(clk_int.mpll_src);

		/* configurate PLL presc */
		main_pll_cfg(clk_int.mpll_q, clk_int.mpll_p, clk_int.mpll_n, clk_int.mpll_m);

		/* export system parameters */
		_sysclk_frq = (((HSE_CLK_SRC/clk_int.mpll_m)*clk_int.mpll_n)/2);

		if (!clk_int.hclk)
			_ahb1_frq = _sysclk_frq;
		else
			_ahb1_frq = _sysclk_frq/clk_int.hclk;
		
		_apb1_frq = _ahb1_frq/clk_int.pclk2;
		_apb2_frq = _apb1_frq*2;

	}

	flash_access_frq(LATENCY_PERIOD_5);

	/* select system clock source */
	switch_sys_clk(clk_int.sysclk_src);
	
	/* dorbid RCC interrupt */
	RCC_CIR = 0x0;

}
#ifdef CONFIG_CLOCK
BOOT_INIT_2(clock_init);
#endif
