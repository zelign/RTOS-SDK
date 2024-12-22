/* Standard includes. */
#include <string.h>
#include <stdint.h>
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
    bool uart_readable_flag = FALSE;
    CLI_Definition_List_Item_t *pxCommand = NULL;
    char *cli_read_buf_p = &cli_read_buf[0];

    printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
    printf(RED_LOG, PREFIX_SEPARATOR);
    printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
    printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);

    for ( ; ;) {
        cli_read = getc_usart1_loop(&uart_readable_flag);
#ifdef XCOM_UART_TOOL
        if (uart_readable_flag) {
            if (cli_read == '\n') {
                if (!strncmp(&cli_read_buf[0], "help", 4)) {
                    printf("\nThis is a help command!\n");
                    memset(&cli_read_buf[0], 0, 64);
                    cli_read_buf_p = &cli_read_buf[0];
                }
                putc_usart1('\r');
                printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
                printf(RED_LOG, PREFIX_SEPARATOR);
                printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
                printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);
            } else if (cli_read == '\r')
                continue;
            else {
                *cli_read_buf_p++ = cli_read;
                putc_usart1(cli_read);
            }
        }
#elif defined(SECURECRT_TOOL)
        if (uart_readable_flag) {
            if (cli_read == '\n') {
                if (cli_read_buf_p > &cli_read_buf[0]) { //confirm that the command only has '\n'
                    *cli_read_buf_p = '\0'; //add the terminal for this command

                    /* traverse the registered command to check if the inputted command is exists */
                    for (pxCommand = get_registered_commands(); pxCommand != NULL; pxCommand = pxCommand->pxNext) {
                        if ((!strncmp(&cli_read_buf[0], pxCommand->pxCommandLineDefinition->pcCommand,
                            strlen(pxCommand->pxCommandLineDefinition->pcCommand))) &&
                            ((cli_read_buf[strlen(pxCommand->pxCommandLineDefinition->pcCommand)] == ' ') || 
                            (cli_read_buf[strlen(pxCommand->pxCommandLineDefinition->pcCommand)] == '\0') ||
                            (cli_read_buf[strlen(pxCommand->pxCommandLineDefinition->pcCommand)] == '\r') ||
                            (cli_read_buf[strlen(pxCommand->pxCommandLineDefinition->pcCommand)] == '\n') )) {
                            /* find the command and process it */
                            FreeRTOS_CLIProcessCommand(pxCommand->pxCommandLineDefinition->pcCommand,
                                &cli_read_buf[strlen(pxCommand->pxCommandLineDefinition->pcCommand)], /* subcommand */
                                strlen(&cli_read_buf[strlen(pxCommand->pxCommandLineDefinition->pcCommand)])); //subcommand length
                            memset(&cli_read_buf[0], 0, 64);
                            cli_read_buf_p = &cli_read_buf[0];
                            break;
                        }
                    }

                    if (!pxCommand) {
                        memset(&cli_read_buf[0], 0, 64);
                        cli_read_buf_p = &cli_read_buf[0];
                        /* Intet prompt */
                        putc_usart1('\r');
                        putc_usart1('\n');
                        printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
                        printf(RED_LOG, PREFIX_SEPARATOR);
                        printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
                        printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);
                        printf("\n Please input correct command:\n");
                        FreeRTOS_CLIProcessCommand("help", NULL, 0);
                    }
                }
				/* Intet prompt */
                putc_usart1('\r');
                putc_usart1('\n');
                printf(GREEN_LOG, STM32F407ZGT6_PREFIX);
                printf(RED_LOG, PREFIX_SEPARATOR);
                printf(LIGHT_BLUE_LOG, STM32F407ZGT6_SUFFIX);
                printf(LIGHT_BLUE_LOG, SUFFIX_SEPARATOR);
            } else if (cli_read == '\r') {
                putc_usart1('\r');
            } else { //store the command into read buffer
                putc_usart1(cli_read);
                if ( cli_read == 0x08 ) { // backspace
                    if (cli_read_buf_p > &cli_read_buf[0]){
                        *cli_read_buf_p-- = 0;
                        putc_usart1(' ');
                        putc_usart1('\b');
                    } else
                        printf("\033[C");
                } else {
                    if (cli_read_buf_p >= &cli_read_buf[62]) {
                        printf("\nThe command is too long!\n");
                        memset(&cli_read_buf[0], 0, 64);
                        cli_read_buf_p = &cli_read_buf[0];
                    } else
                        *cli_read_buf_p++ = cli_read;
                }
            }
        }
#endif
        portYIELD();
        // vTaskDelay(50);
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

