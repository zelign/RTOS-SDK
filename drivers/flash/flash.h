#ifndef __FLASH_H__
#define __FLASH_H__

#include "system.h"
#include "board_init.h"
#include "../../output/config.h"
#include "spi.h"
#include "gpio.h"

#define SPI1_GPIO       GPIOB
#define FLASH_CS_Pin    GPIO_PIN_14
#define SPI1_MISO       GPIO_PIN_4
#define SPI1_MOSI       GPIO_PIN_5
#define SPI1_SCK        GPIO_PIN_3

struct flash_info {
    const char *dev_name;
    unsigned int dev_id;
    unsigned int chip_size; //KB
    unsigned int block_size; //KB
    unsigned char sector_size; //KB
    unsigned char page_num;
};

struct flash_partition {
    char *name;
    unsigned int offset;
    unsigned int size;
};

#define PAGE_PROGRAM(sd, cmd, add, src, src_len, function) \
        function(sd, &cmd); \
        cmd = (unsigned char)(add >> 16); \
        function(sd, &cmd); \
        cmd = (unsigned char)(add >> 8); \
        function(sd, &cmd); \
        cmd = (unsigned char)(add >> 0); \
        function(sd, &cmd); \
        for (unsigned int i = 0; i < src_len; i++){ \
            function(sd, &src[i]); }
#define ERASE_ALL(sd) \
    sm_printf("Erase all chip ... \n"); \
    by25q64as_chip_erase(sd); \
    sm_printf("Over! \n"); \

#ifdef CONFIG_BY25Q64AS

#define FLASH_NAME "BY25Q64AS"
void spi_1_by25q64as_init(void);
#endif // CONFIG_BY25Q64AS

#endif
