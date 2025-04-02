#include "exti.h"
#include "nvic.h"
#include "led.h"

static void exti_source(enum gpio_reg gpio, enum gpio_pin pin)
{
	*(volatile unsigned int *)(SYSCFG_EXTICR + ((pin / 4) * 4)) &=
		~(0xf << ((pin % 4) * 4));
	*(volatile unsigned int *)(SYSCFG_EXTICR + ((pin / 4) * 4)) |=
		((gpio / 0x400) << ((pin % 4) * 4));
}

static void exti_falling_edge_trigger(unsigned char line, bool flag)
{
	if (flag)
		*(volatile unsigned int *)EXTI_FTSR |= (ENABLE << line);
	else
		*(volatile unsigned int *)EXTI_FTSR &= ~(ENABLE << line);
}

static void exti_rising_edge_trigger(unsigned char line, bool flag)
{
	if (flag)
		*(volatile unsigned int *)EXTI_RTSR |= (ENABLE << line);
	else
		*(volatile unsigned int *)EXTI_RTSR &= ~(ENABLE << line);
}

static void exti_int_mask(unsigned char line, bool flag)
{
	if (flag)
		*(volatile unsigned int *)EXTI_IMR |= (ENABLE << line);
	else
		*(volatile unsigned int *)EXTI_IMR &= ~(ENABLE << line);
}

static void exti_evt_mask(unsigned char line, bool flag)
{
	if (flag)
		*(volatile unsigned int *)EXTI_EMR |= (ENABLE << line);
	else
		*(volatile unsigned int *)EXTI_EMR &= ~(ENABLE << line);
}

static bool exti_pend_state(unsigned char line)
{
	if ((*(volatile unsigned int *)(EXTI_PR)) & (ENABLE << line))
		return ENABLE;
	return DISABLE;
}

/* enable syscfg clock */
static void syscfg_clk_enable(void)
{
	*(volatile unsigned int *)RCC_APB2ENR |= (1 << 14);
}

static void exti_clear_flag(unsigned char line)
{
	*(volatile unsigned int *)EXTI_PR |= (ENABLE << line);
}

void exti_init_gpio(enum EXC_NUM exc_num, enum priority_group prio_grp,
		    unsigned char empt_prio, unsigned char sub_prio,
		    enum gpio_reg gpio, enum gpio_pin pin, bool fall_edge,
		    bool rise_edge, bool mask)
{
	syscfg_clk_enable();
	NVIC_init(exc_num, prio_grp, empt_prio, sub_prio);
	exti_source(gpio, pin);
	exti_falling_edge_trigger(pin, fall_edge);
	exti_rising_edge_trigger(pin, rise_edge);
	exti_int_mask(pin, mask);
	exti_evt_mask(pin, DISABLE);
}

static bool led_status = ENABLE;
void EXTI0_IRQHandler(void)
{
	if (exti_pend_state(0)) {
		exti_clear_flag(0);
		if (led_status) {
			led_light(LED_RED);
			led_status = DISABLE;
		} else {
			led_off(LED_RED);
			led_status = ENABLE;
		}
	}
}
