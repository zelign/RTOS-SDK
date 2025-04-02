/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "nvic.h"
#include "initcall.h"
#include "system.h"

void INT_disable(enum EXC_NUM exc_num)
{
	/* disable interrupt */
	*((volatile unsigned int *)(ICER_BASE_ADDR + ((exc_num / 32) * 4))) &=
		~(0x1 << ((exc_num % 32) - 1));
}

void set_pending_state(enum EXC_NUM exc_num)
{
	/* set pending state */
	*((volatile unsigned int *)(ISPR_BASE_ADDR + ((exc_num / 32) * 4))) |=
		((0x1 << ((exc_num % 32) - 1)));
}

void clear_pending_state(enum EXC_NUM exc_num)
{
	/* clear pending state */
	*((volatile unsigned int *)(ICPR_BASE_ADDR + ((exc_num / 32) * 4))) |=
		((0x1 << ((exc_num % 32) - 1)));
}

/**
 * 
 * return: 0 no-pending 1 pending
*/
unsigned char get_pending_state(enum EXC_NUM exc_num)
{
	return ((*((volatile unsigned int *)(ISPR_BASE_ADDR +
					     ((exc_num / 32) * 4))) &
		 ((0x1 << ((exc_num % 32) - 1)))) ?
			1 :
			0);
}

/**
 * 
 * return: 0 no 1 active
*/
unsigned char get_active_state(enum EXC_NUM exc_num)
{
	return ((*((volatile unsigned int *)(IABR_BASE_ADDR +
					     ((exc_num / 32) * 4))) &
		 ((0x1 << ((exc_num % 32) - 1)))) ?
			1 :
			0);
}

void set_priority_group(enum priority_group pri)
{
	*(volatile unsigned int *)AIRCR &= ~(0x07 << 8);
	*(volatile unsigned int *)AIRCR |= (pri << 8);
}

static void _set_priority(enum EXC_NUM exc_num, unsigned char prio)
{
	*((volatile unsigned int *)(IP_BASE_ADDR + (exc_num / 4) * 4)) &=
		~(0xFF << (exc_num % 4) * 8);
	*((volatile unsigned int *)(IP_BASE_ADDR + (exc_num / 4) * 4)) |=
		(prio << (exc_num % 4) * 8);
}

void set_priority(enum priority_group prio_grp, unsigned char empt_prio,
		  unsigned char sub_prio, enum EXC_NUM exc_num)
{
	unsigned char sub_prio_num = 0;
	unsigned char empt_prio_num = 0;
	unsigned char prio = 0;

	/* Get the bit number of emption and sub-priority priority */
	sub_prio_num = prio_grp >> (prio_grp - 1);
	empt_prio_num = PRIORITY_MAX_BIT_NUM - sub_prio_num;

	/* caculate the priority */
	prio = (empt_prio << (8 - empt_prio_num));
	prio |= (sub_prio << PRIORITY_MAX_BIT_NUM);

	set_priority_group(prio_grp);

	/* set the priority */
	_set_priority(exc_num - 16, prio);
}

void NVIC_init(enum EXC_NUM exc_num, enum priority_group prio_grp,
	       unsigned char empt_prio, unsigned char sub_prio)
{
	set_priority(prio_grp, empt_prio, sub_prio, exc_num);
	NVIC_ENABLE(exc_num - 16);
}

static void system_handler_priority(void)
{
	/* MemManage_Handler priority: 0 */
	*(volatile unsigned int *)SHPR_1 &= ~0xff;
	*(volatile unsigned int *)SHPR_1 |= 0x0;

	/* BusFault_Handler priority: 1 */
	*(volatile unsigned int *)SHPR_1 &= ~(0xff << 0x08);
	*(volatile unsigned int *)SHPR_1 |= (0x01 << 0x08);

	/* UsageFault_Handler priority: 2 */
	*(volatile unsigned int *)SHPR_1 &= ~(0xff << 0x10);
	*(volatile unsigned int *)SHPR_1 |= (0x02 << 0x10);

	/* SVC_Handler priority: 3 */
	*(volatile unsigned int *)SHPR_2 &= ~(0xff << 0x18);
	*(volatile unsigned int *)SHPR_2 |= (0x03 << 0x18);

	/* DebugMon_Handler priority: 4 */
	*(volatile unsigned int *)SHPR_3 &= ~(0xff);
	*(volatile unsigned int *)SHPR_3 |= (0x04);

	/* PendSV_Handler priority: 5 */
	*(volatile unsigned int *)SHPR_3 &= ~(0xff << 0x10);
	*(volatile unsigned int *)SHPR_3 |= (0x05 << 0x10);

	/* SysTick_Handler priority: 6 */
	*(volatile unsigned int *)SHPR_3 &= ~(0xff << 0x18);
	*(volatile unsigned int *)SHPR_3 |= (0x06 << 0x18);
}

BOOT_INIT_1(system_handler_priority);

/*
 * TRUE: unprivilege
 * FALSE: privilege
**/
void get_privilege_level(void)
{
	unsigned int control;
	__asm volatile("mrs %0, control" : "=r"(control) : :);
	__asm volatile("isb");
	((control & 0x1) ? puts_usart1("unprivilege!\n") :
			   puts_usart1("privilege!\n"));
	return;
}

/*
 * TRUE: unprivilege
 * FALSE: privilege
**/
void set_privilege_level(bool level)
{
	unsigned int control = 0;

	__asm volatile("mrs %0, control" : "=r"(control) : :);

	control |= level;

	__asm volatile("msr control, %0" : : "r"(control) :);
	level ? puts_usart1("set unprivilege!\n") :
		puts_usart1("set privilege!\n");
}

void get_sp_mode(void)
{
	unsigned int control;
	__asm volatile("mrs %0, control" : "=r"(control) : :);

	((control & 0x2) ? puts_usart1("PSP!\n") : puts_usart1("MSP!\n"));
	return;
}

void set_sp_level(bool mode)
{
	unsigned int control = 0;

	__asm volatile("mrs %0, control" : "=r"(control) : :);

	control |= mode << 1;

	__asm volatile("msr control, %0" : : "r"(control) :);
	mode ? puts_usart1("set PSP!\n") : puts_usart1("set MSP!\n");
	return;
}

/*
 * TRUE: PSP
 * FALSE:MSP
*/
unsigned int get_sp(bool mpsp)
{
	unsigned int sp;
	switch (mpsp) {
	case TRUE:
		__asm volatile("mrs %0, MSP" : "=r"(sp) : :);
		break;
	case FALSE:
		__asm volatile("mrs %0, PSP" : "=r"(sp) : :);
		break;
	default:
		return 0xffff;
	}
	return sp;
}

unsigned char get_exception_num(void)
{
	unsigned int xpsr;
	__asm volatile("mrs %0, xpsr" : "=r"(xpsr) : :);
	__asm volatile("isb");

	return (unsigned char)(xpsr & 0xff);
}

unsigned int get_lr_reg(void)
{
	unsigned int lr;
	__asm volatile("mov %0, lr" : "=r"(lr) : :);
	return lr;
}
