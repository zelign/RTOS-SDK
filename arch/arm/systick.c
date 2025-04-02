/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "nvic.h"
#include "clock.h"
#include "initcall.h"

static volatile unsigned int _systick_cnt;

static void enable_systick(void)
{
	/* set the SYST_CSR->ENABLE(bit[0]) */
	*(volatile unsigned int *)SYST_CSR |= 0x1;
}

/**
 * val=1/SYSCLK(ststick source=system clock)
*/
void set_systick_reload_val(unsigned int val)
{
	/* set the reload value to SYST_RVR[24:0] */
	*(volatile unsigned int *)SYST_RVR &= 0x0;
	*(volatile unsigned int *)SYST_RVR |= (val - 1);
}

static inline void systick_exception(bool exp)
{
	/* set the SYST_CSR->TICKINT(bit[1]) */
	if (exp)
		*(volatile unsigned int *)SYST_CSR |= 0x2;
	else
		*(volatile unsigned int *)SYST_CSR &= ~0x2;
}

unsigned char read_count_flag(void)
{
	return ((*(volatile unsigned int *)SYST_CSR) >> 16);
}

unsigned int read_current_count(unsigned int *cnt)
{
	return (*(volatile unsigned int *)SYST_CVR);
}

/**
 * 1 internal 0 external
*/
static void set_clk_source(bool source)
{
	if (source)
		*(volatile unsigned int *)SYST_CSR |= 0x04;
	else
		*(volatile unsigned int *)SYST_CSR &= ~(0x04);
}

void systick_init(unsigned int reload_val, bool clk_src, bool exp_enable,
		  unsigned char prio)
{
	/* set the reload value */
	set_systick_reload_val(reload_val);

	/* clean all state */
	*(volatile unsigned int *)SYST_CSR &= 0x0;

	/* clean the systic current value register */
	*(volatile unsigned int *)SYST_CVR &= 0x0;

	/* set the clock source */
	set_clk_source(clk_src);

	/* wether enable exception */
	systick_exception(exp_enable);

	/* enable the systick */
	enable_systick();
}

void boot_systick_init(void)
{
	systick_init((168000 - 1), ENABLE, DISABLE, SYS_STK);
}

//BOOT_INIT_1(boot_systick_init);

void systick_delay_ms(unsigned int ms)
{
	systick_init((168000 - 1), ENABLE, ENABLE, SYS_STK);
	_systick_cnt = ms;
	while (1)
		if (!_systick_cnt)
			break;
}

void systick_delay_us(unsigned int us)
{
	systick_init((168 - 1), ENABLE, ENABLE, SYS_STK);
	_systick_cnt = us;
	while (_systick_cnt--)
		;
}

// void SysTick_Handler(void)
// {
//     if (_systick_cnt)
//         _systick_cnt--;
//     xPortSysTickHandler();
// }
