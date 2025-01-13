#ifndef __FLASH_H__
#define __FLASH_H__

#include "system.h"
#include "board_init.h"
#include "autoconfig.h"
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
    unsigned int chip_size; //chip size(KB)
    unsigned int block_size; //every block size(KB)
    unsigned char sector_size; //every sector size(KB)
    unsigned char page_num; //page numbers per sector
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

/* Flash commands defination */
#define PAGE_PROGRAM_CMD    0x02
#define FAST_PAGE_PROGRAM_CMD   0xF2
#define WRITE_ENABLE_CMD    0x06
#define WRITE_DISABLE_CMD   0x04

#define READ_DATA_CMD       0x03
#define FAST_READ_CMD       0x0B


#define SECTOR_ERASE_CMD    0x20
#define BLOCK_ERASE_CMD     0x52
#define CHIP_ERASE_CMD      0xD8

#define ENABLE_RESET_CMD        0x66
#define RESET_CMD               0x99
#define MANUFACTURER_CMD        0x90

#define FLASH_NAME "BY25Q64AS"

void spi_1_by25q64as_init(void);
void by25q64as_read_data(enum spi_dev sd, unsigned int address, char *data, unsigned int data_len);
int by25q64as_read_write(enum spi_dev sd, unsigned int address, void *data, unsigned int data_len);
void by25q64as_sector_erase(enum spi_dev sd, unsigned int address);
void by25q64as_chip_erase(enum spi_dev sd);

#endif // CONFIG_BY25Q64AS

#endif
