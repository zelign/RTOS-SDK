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

#ifdef COLOR_LOG
#define COLOR_LOG "\033[1;31m%s\033[0m"
#else
#define COLOR_LOG
#endif

#define STM32F407ZGT6_PREFIX    "stm32f407"
#define PREFIX_SEPARATOR        "@"
#define STM32F407ZGT6_SUFFIX    "iris"
#define SUFFIX_SEPARATOR        ":/"

static char cli_read_buf[64];

void cli_executor_loop( void *para )
{
    (void) para;
    char cli_read = 0;
    bool flag = FALSE;
    char *rbuf = &cli_read_buf[0];

    sm_printf("%s", STM32F407ZGT6_PREFIX);
    sm_printf("%s", PREFIX_SEPARATOR);
    sm_printf("%s", STM32F407ZGT6_SUFFIX);
    sm_printf("%s", SUFFIX_SEPARATOR);
    for ( ; ;) {
        // sm_printf("stm32f407@iris:/%d\n", strlen(STM32F407ZGT6_PREFIX));
        cli_read = getc_usart1_loop(&flag);
        if (flag == TRUE) {
            if (cli_read == '\n') {
                if (!strncmp(&cli_read_buf[0], "help", strlen("help"))) {
                    sm_printf("\nThis is a help command!\n");
                    memset(&cli_read_buf[0], 0, 64);
                    rbuf = &cli_read_buf[0];
                }
                putc_usart1('\r');
                sm_printf("%s", STM32F407ZGT6_PREFIX);
                sm_printf("%s", PREFIX_SEPARATOR);
                sm_printf("%s", STM32F407ZGT6_SUFFIX);
                sm_printf("%s", SUFFIX_SEPARATOR);
            } else if (cli_read == '\r')
                continue; 
            else {
                *rbuf++ = cli_read;
                putc_usart1(cli_read);
            }
        }
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

