/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "usart.h"
#include "initcall.h"
#include "gpio.h"
#include "led.h"
#include "system.h"
#include "autoconfig.h"

#define RECEIVE_SIZE 128
// #define CONFIG_USART_IRQ_TRIGGER 1

static char receive_array[RECEIVE_SIZE];

static struct _usart1_t {
    volatile unsigned char index;
    char *priv;
#ifdef CONFIG_USART_IRQ_TRIGGER
    volatile unsigned short getc_buf;
    volatile bool putc_flag;
    volatile bool getc_flag;
    volatile bool puts_flag;
#endif
} usart1_t = {
#ifdef CONFIG_USART_IRQ_TRIGGER
    .putc_flag = FALSE,
    .getc_flag = FALSE,
    .puts_flag = FALSE,
#endif
    .index = 0,
    .priv = receive_array,
};

static inline void set_baud_rate(unsigned int baud)
{
    unsigned char over8 = ((USART1_CR1 >> 15) & (0x1));
    unsigned int div_m = (84*1000000)/(baud * 8 * (2 - over8));
    unsigned int div_f = (unsigned char)(((((double)_apb2_frq*1000000)/(/*(double)*/(baud * 8 * (2 - over8)))) - div_m) * 16);
    USART1_BRR &= ~(0xffff);
    USART1_BRR |= (div_m << 4);
    USART1_BRR |= div_f;
}

static inline void oversamping_mode(enum oversamping_mode mod)
{
    USART1_CR1 &= ~(0x1 << 15);
    USART1_CR1 |= (mod << 15);
    USART1_SR &= ~(0x1 << 6);
}

static inline void word_length(enum word_length wl)
{
    USART1_CR1 &= ~(0x1 << 12);
    USART1_CR1 |= (wl << 12);
}

static inline void usart_pin_ctl(enum gpio_reg gpio, enum gpio_pin pin, enum gpio_afr afr, enum gpio_moder moder)
{
    gpio_port_set(gpio, pin, moder, PUSH, GPIO_50MHZ, UP_PULL); 
}

static inline void usart1_reuse_ctl(enum gpio_reg gpio, enum gpio_pin pin, enum gpio_afr afr, enum gpio_moder moder)
{
    usart_pin_ctl(gpio, pin, afr, moder);
}

static inline void set_stop_bit(enum usart_stop_bit bit)
{
    USART1_CR2 &= ~(0x3 << 12);
    USART1_CR2 |= (bit << 12);
}



void usart1_init()
{
    enable_gpio_clock(GPIOA);
    USART1_ENABLE_CLK;

  /* TX--AF9 */
    pin_afr(GPIOA, GPIO_PIN_9, AF7);
    pin_afr(GPIOA, GPIO_PIN_10, AF7);
    gpio_pin_cfg(GPIOA,GPIO_PIN_9, RESET);
    usart1_reuse_ctl(GPIOA,GPIO_PIN_9,AF7,REUSE);

    /* RX--AF10 */
    usart1_reuse_ctl(GPIOA,GPIO_PIN_10,AF7,REUSE);
    USART1_CR1 &= 0x0;

    oversamping_mode(over_16);
    word_length(word_length_8);
    set_stop_bit(STOP_1);
    set_baud_rate(9600);

    USART1_DISABLE_PARITY;
    USART1_ENABLE_RE;
    USART1_ENABLE_TE;

#ifdef CONFIG_USART_IRQ_TRIGGER
    // USART1_ENABLE_TXE_IE;
    USART1_ENABLE_RXNE_IE;
    USART1_ENABLE_IDLE_IE;
    // USART1_ENABLE_TC_IE;
    NVIC_init(INT_USART1, EMPT_1_SUB_3, 1,3);
#endif

    USART1_CR3 &= 0;
    USART1_ENABLE_UE;

    printf("\nUart 1 intialize successful!\n");
   
}

void putc_usart1(char c)
{
    while(((USART1_SR & 0x80) == 0));
    USART1_DR = (c & 0xff);   
}

void put32_usart1(int i)
{
    char _i, flag = 3;
    do {
        _i = (unsigned char)(i >> flag * 8);
        putc_usart1(_i);
    } while(flag-- != 0);
}

void puts_usart1(char *s)
{
    do {
        // USART1_DR = *s++;
        putc_usart1(*s++);
        // while(((USART1_SR & 0x80) == 0));
    } while((*s != '\0'));
    USART1_DR = 0x0D;
    while(((USART1_SR >> 6) & 0x1) != TRUE);
}

char getc_usart1_loop (bool *flag)
{
    // while ((((USART1_SR >> 4) & 1) != TRUE) || (((USART1_SR >> 5) & 1) != TRUE));
    if (((USART1_SR >> 5) & 1) != TRUE) {
        *flag = FALSE;
        return 0;
    }
    *flag = TRUE;
    return USART1_DR & (0xff);
}

#ifndef CONFIG_USART_IRQ_TRIGGER

static inline unsigned short getc_usart1 (void)
{
    while((((USART1_SR >> 4) & 1) != TRUE) || (((USART1_SR >> 5) & 1) != TRUE));
    return USART1_DR & (0x1ff);
}

char *gets_usart1 (void)
{
    for (usart1_t.index = 0; (usart1_t.index < RECEIVE_SIZE); \
        usart1_t.index++) {
            receive_array[usart1_t.index] = (unsigned short)getc_usart1();
            if (receive_array[usart1_t.index] == '\0')
                break;
        }
    return receive_array;
}

#else

static inline unsigned short getc_usart1 (void)
{
    while((((USART1_SR >> 4) & 1) != TRUE) || (((USART1_SR >> 5) & 1) != TRUE));
    return USART1_DR & (0x1ff);
}

char *gets_usart1(void)
{
    while(!usart1_t.getc_flag);
    usart1_t.getc_flag = FALSE;
    usart1_t.index = 0;
    return receive_array;
}

void USART1_IRQHandler(void)
{
    if (((USART1_SR >> 5) & 1)) {
        receive_array[usart1_t.index++] = USART1_DR;
    }

    if (((USART1_SR >> 4) & 1)) {
        receive_array[usart1_t.index++] = USART1_DR;
        receive_array[usart1_t.index] = '\0';
        usart1_t.getc_flag = TRUE;
    }
}
#endif

#ifdef CONFIG_USART
BOOT_INIT_2(usart1_init);
#endif