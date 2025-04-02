#ifndef __EXTI_H__
#define __EXTI_H__

#include "board_cfg.h"
#include "nvic.h"

void exti_init_gpio(enum EXC_NUM exc_num, enum priority_group prio_grp,
		    unsigned char empt_prio, unsigned char sub_prio,
		    enum gpio_reg gpio, enum gpio_pin pin, bool fall_edge,
		    bool rise_edge, bool mask);

#endif
