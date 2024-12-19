/*
 * Copyright (c) 2023-2024 Simon. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "system.h"
#include "board_init.h"
#include "gpio.h"
#include "led.h"
#include "key.h"
#include "nvic.h"
#include "exti.h"
#include "usart.h"
#include "../output/config.h"
#include "FreeRTOS.h"
#include "spi.h"
#include "task.h"

#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS
#include "flash.h"
#endif
#endif

void basic_server(void *par)
{
	(void)par;
	app_init();
#ifdef CONFIG_LED
	led_init(LED_RED, SET);
    led_light(LED_RED);
#endif

#ifdef CONFIG_KEY
	key_init(KEY_PORT_UP);
#endif

#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS
    spi_1_by25q64as_init();
#endif
#endif
	exti_init_gpio(INT_EXT0, EMPT_1_SUB_3, 0, 2, KEY_PORT_UP, KEY_UP_PIN_UP, DISABLE, ENABLE, ENABLE);
	sm_printf("Welcome to the MicroSDK!\n");
    cli_console_init();
	vTaskDelete(NULL);
}

void test1(void *par)
{
	(void)par;
	while (1) {
        sm_printf("[test] %s\n", __func__);
		led_off(LED_RED);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	vTaskDelete(NULL);
}

void test2(void *par)
{
	(void)par;
	while (1) {
        sm_printf("[test] %s\n", __func__, __LINE__);
		vTaskDelay(pdMS_TO_TICKS(200));
        led_light(LED_RED);
	}

	vTaskDelete(NULL);
}

int main(int c, char *argv[])
{
#if 0
	put32_usart1(get_sp_reg());
	app_init();
	led_init(LED_RED, SET);
	key_init(KEY_PORT_UP);
	led_light(LED_RED);
	exti_init_gpio(INT_EXT0, EMPT_1_SUB_3, 0, 2, KEY_PORT_UP, KEY_UP_PIN_UP, DISABLE, ENABLE, ENABLE);
	sm_printf("Welcome to the MicroSDK!\n");

#else

    puts_usart1("Start To FreeRTOS Task Creating!");
	xTaskCreate( basic_server,
                "basi",
                configMINIMAL_STACK_SIZE,
                NULL,
                configMAX_PRIORITIES - 1,
                NULL);
	// xTaskCreate( test1,
    //             "test1",
    //             configMINIMAL_STACK_SIZE,
    //             NULL,
    //             configMAX_PRIORITIES - 2,
    //             NULL);
    // xTaskCreate( test2,
    //             "test2",
    //             configMINIMAL_STACK_SIZE,
    //             NULL,
    //             configMAX_PRIORITIES - 3,
    //             NULL);

    /* Start the scheduler. */
    sm_printf("Start Task Schedule!\n");
    vTaskStartScheduler();
#endif
	while(1);
}

void vApplicationIdleHook(void)
{
    sm_printf("This is a Idle Task Hook function!\n");
}

void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                        StackType_t ** ppxIdleTaskStackBuffer,
                                        uint32_t * pulIdleTaskStackSize )
{
     /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
