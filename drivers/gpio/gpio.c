/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "gpio.h"
#include "initcall.h"

void gpio_init(enum gpio_reg gpio, enum gpio_pin pin,
               enum gpio_moder mod, enum gpio_otype typ,
               enum gpio_speed spd, enum gpio_uppull pul,
               enum gpio_set gset)
{
    /* Enable clock */
    enable_gpio_clock(gpio);

    gpio_port_set(gpio, pin, mod, typ, spd, pul);

    gpio_pin_cfg(gpio, pin, gset);
}

void gpio_port_set(enum gpio_reg GPIO,
                   enum gpio_pin pin,
                   enum gpio_moder moder,
                   enum gpio_otype oty,
                   enum gpio_speed speed,
                   enum gpio_uppull pull)
{
    /* configurate port modle */
    if (moder != -1) {
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + MODER)) &= ~(0x3 << (pin * 2));
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + MODER)) |= (moder << (pin * 2));
    }
    
    if (oty != -1) {
        /* configurate port output type */
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + OTYPER)) &= ~(1 << pin);
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + OTYPER)) |= (oty << pin);
    }
    
    if (speed != -1) {
        /* configurate port speed */
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + OSPEEDER)) &= ~(0x3 << pin * 2);
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + OSPEEDER)) |= (speed << pin * 2);        
    }

    if (pull != -1) {
        /* configurate up/down pull */
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + PUPDR))&= ~(0x3 << pin * 2);
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + PUPDR)) |= (pull << pin * 2);      
    }
}

void gpio_pin_cfg(enum gpio_reg GPIO, enum gpio_pin pin, enum gpio_set gset)
{ 
    if (gset)
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + BSRR)) |= (1 << pin);
    else
        *((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + BSRR)) |= (1 << (16+pin));
}

uint8_t gpio_read(enum gpio_reg GPIO, enum gpio_pin pin)
{
    return (((*((uint32_t volatile *)(GPIO_BASE_ADDR + GPIO + IDR))) >> pin) & 0x01);
}


void enable_gpio_clock (enum gpio_reg gpio)
{
    if (gpio == 0) {
        RCC_AHB1ENR &= ~0x1;
        RCC_AHB1ENR |= (1 << gpio);
    } else {
        RCC_AHB1ENR &= ~(1 << (gpio/1024));
        RCC_AHB1ENR |= (1 << (gpio/1024));
    }
}

void pin_afr(enum gpio_reg gpio, enum gpio_pin pin, enum gpio_afr afr)
{
    if (pin < GPIO_PIN_8) {
        *((volatile unsigned int *)(GPIO_BASE_ADDR + gpio + AFRL)) &= ~(0xf << (pin * 4));
        *((volatile unsigned int *)(GPIO_BASE_ADDR + gpio + AFRL)) |= (afr << (pin * 4));
    } else {
        *((volatile unsigned int *)(GPIO_BASE_ADDR + gpio + AFRH)) &= ~(0xf << ((pin - 8) * 4));
        *((volatile unsigned int *)(GPIO_BASE_ADDR + gpio + AFRH)) |= (afr << ((pin - 8) * 4));
    }
    
}

