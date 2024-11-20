/* Standard includes. */
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "system.h"

/* Utils includes. */
#include "FreeRTOS_CLI.h"

#include "../../output/config.h"

#define RED_LOG "\033[1;31m%s\033[0m"
#define GREEN_LOG "\033[1;32m%s\033[0m"
#define YELLOW_LOG "\033[1;33m%s\033[0m"
#define BLUE_LOG "\033[1;34m%s\033[0m"
#define PURPLE_LOG "\033[1;35m%s\033[0m"
#define LIGHT_BLUE_LOG "\033[1;36m%s\033[0m"
#define WHITE_LOG "\033[1;37m%s\033[0m"
#define GRAY_LOG "\033[1;38m%s\033[0m"
#define BLACK_LOG "\033[1;39m%s\033[0m"

#define STM32F407ZGT6_PREFIX    "stm32f407"
#define PREFIX_SEPARATOR        "@"
#define STM32F407ZGT6_SUFFIX    "iris"
#define SUFFIX_SEPARATOR        ":/"

#define SECURECRT_TOOL

static char cli_read_buf[64];

void cli_executor_loop( void *para )
{
    (void) para;
    char cli_read = 0;
    bool flag = FALSE;
    char *rbuf = &cli_read_buf[0];

    sm_printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
    sm_printf(RED_LOG, PREFIX_SEPARATOR);
    sm_printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
    sm_printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);
    for ( ; ;) {
        // sm_printf("stm32f407@iris:/%d\n", strlen(STM32F407ZGT6_PREFIX));
        cli_read = getc_usart1_loop(&flag);
#ifdef XCOM_UART_TOOL
        if (flag) {
            if (cli_read == '\n') {
                if (!strncmp(&cli_read_buf[0], "help", 4)) {
                    sm_printf("\nThis is a help command!\n");
                    memset(&cli_read_buf[0], 0, 64);
                    rbuf = &cli_read_buf[0];
                }
                putc_usart1('\r');
                sm_printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
                sm_printf(RED_LOG, PREFIX_SEPARATOR);
                sm_printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
                sm_printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);
            } else if (cli_read == '\r')
                continue;
            else {
                *rbuf++ = cli_read;
                putc_usart1(cli_read);
            }
        }
#elif defined(SECURECRT_TOOL)
        if (flag) {
            if (cli_read == '\n') {
                if (!strncmp(&cli_read_buf[0], "help", 4)) {
                    sm_printf("\nThis is a help command!\n");
                    memset(&cli_read_buf[0], 0, 64);
                    rbuf = &cli_read_buf[0];
                }
                putc_usart1('\r');
                putc_usart1('\n');
                sm_printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
                sm_printf(RED_LOG, PREFIX_SEPARATOR);
                sm_printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
                sm_printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);
            } else if (cli_read == '\r') {
                putc_usart1('\r');
            } else {
                putc_usart1(cli_read);
                if ( cli_read == 0x08 ) {
                    if (rbuf > &cli_read_buf[0]){
                        *rbuf-- = 0;    
                        putc_usart1(' ');
                        putc_usart1('\b');
                    } else
                        sm_printf("\033[C");
                } else
                    *rbuf++ = cli_read;
            }
        }
#endif
        portYIELD();
    }

}

void cli_console_init(void)
{
    xTaskCreate( cli_executor_loop,
                "cli executor",
                configMINIMAL_STACK_SIZE * 4,
                NULL,
                configMAX_PRIORITIES - 4,
                NULL);
}

