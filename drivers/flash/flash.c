#include "flash.h"
#include <string.h>
#include <stdlib.h>

static struct flash_info flash_informations[] = 
{
#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS
    {
        .dev_name = FLASH_NAME,
        .dev_id = 0x6816,
        .chip_size = 8192,
        .block_size = 64,
        .sector_size = 4,
        .block_num = 128,
        .sector_num = 16,
    },
#endif
#endif
};

static inline void spi_reuse_cfg(enum gpio_reg GPIO,
                                enum gpio_pin pin,
                                enum gpio_moder moder,
                                enum gpio_otype oty,
                                enum gpio_speed speed,
                                enum gpio_uppull pull)
{
    gpio_port_set(GPIO, pin, moder, oty, speed, pull);
}

#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS

void by25q64as_page_program(enum spi_dev sd, unsigned char *address, void *data, unsigned int data_len)
{
    unsigned char page_program_cmd = 0x02;
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (*address & 0xff) {
        printf("Please alignment the address to page band (0x100)\n");
        return;
    }
    if (! spi_chk_buy(sd)) {
        spi_trans(sd, &page_program_cmd);
        spi_trans(sd, address++);
        spi_trans(sd, address++);
        spi_trans(sd, address);

        for (unsigned int i = 0; i < data_len; i++) {
            spi_trans(sd, (data + i));
        }
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}


void by25q64as_read_dara(enum spi_dev sd, unsigned char *address, unsigned char *data, unsigned int data_len)
{
    unsigned char read_data_cmd = 0x03;
    unsigned char write_cmd = 0xff;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (*address & 0xff) {
        printf("Please alignment the address to page band (0x100)\n");
        return;
    }
    if (! spi_chk_buy(sd)) {
        spi_trans(sd, &read_data_cmd);
        spi_trans(sd, address++);
        spi_trans(sd, address++);
        spi_trans(sd, address);

        for (unsigned int i = 0; i < data_len; i++)
            *(data ++) = spi_trans(sd, &write_cmd);
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

static void by25q64as_write_enable(void)
{
    unsigned char write_enable_cmd = 0x06;
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (! spi_chk_buy(SPI_1))
        spi_trans(SPI_1, &write_enable_cmd); //get manufacture//get device id
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

static void by25q64as_gpio_init(void)
{
    //CS: normal mode
    gpio_init(SPI1_GPIO, FLASH_CS_Pin, OUTPUT, PUSH, GPIO_50MHZ, UP_PULL, SET);

    //CLK: alternate, the clock of GPIOB has been enabled by the gpio_init function
    pin_afr(SPI1_GPIO, SPI1_SCK, AF5);
    //MISO: alternate
    pin_afr(SPI1_GPIO, SPI1_MISO, AF5);
    //MOSI: alternate
    pin_afr(SPI1_GPIO, SPI1_MOSI, AF5);

    spi_reuse_cfg(SPI1_GPIO, SPI1_SCK, REUSE, PUSH, GPIO_50MHZ, UP_PULL);
    // gpio_pin_cfg(SPI1_GPIO, SPI1_SCK, RESET);
    
    spi_reuse_cfg(SPI1_GPIO, SPI1_MISO, REUSE, PUSH, GPIO_50MHZ, UP_PULL);
    // gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    
    spi_reuse_cfg(SPI1_GPIO, SPI1_MOSI, REUSE, PUSH, GPIO_50MHZ, UP_PULL);
    // gpio_pin_cfg(SPI1_GPIO, SPI1_MOSI, RESET);
}

static void get_by25q64as_device_id(unsigned char *dev_id)
{
    unsigned char get_id_cmd[5];
    get_id_cmd[0] = 0x90;
    get_id_cmd[1] = 0x0;
    get_id_cmd[2] = 0x0;
    get_id_cmd[3] = 0x0;
    get_id_cmd[4] = 0xff;
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (! spi_chk_buy(SPI_1)) {
        spi_trans(SPI_1, &get_id_cmd[0]); //get manufacture
        spi_trans(SPI_1, &get_id_cmd[1]);
        spi_trans(SPI_1, &get_id_cmd[2]);
        spi_trans(SPI_1, &get_id_cmd[3]); //get device id
        *dev_id++ = spi_trans(SPI_1, &get_id_cmd[4]);
        *dev_id = spi_trans(SPI_1, &get_id_cmd[4]);
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void spi_1_by25q64as_init(void)
{
    unsigned char id[2];
    unsigned short temp = 0;
    unsigned char test_adr[3];
    unsigned char *read_data = NULL;

    SPI1_CLK_ENABLE;
    by25q64as_gpio_init();
    SPI1_RESET;
    spi_init(SPI_1,
            double_line_single_direct,
            send_rev_select_8_bit,
            enable_software_management,
            first_msb,
            pclk_divide_256,
            master_mode,
            motorola_mode,
            free_lower_first_edge_sample,
            TRUE);
    get_by25q64as_device_id(&id[0]);
    temp |= id[0] << 8;
    temp |= id[1];
    struct flash_info *f = &flash_informations[0];
    for (unsigned int i = 0; i < sizeof(flash_informations)/sizeof(struct flash_info); i++) {
        if (f->dev_id == temp) {
            printf("----------------------------%s----------------------------\n", f->dev_name);
            printf("Chip Size: %dKB\n", f->chip_size);
            printf("Flash ID: 0X%X\n", temp);
            printf("Block Size: %dKB\n", f->block_size);
            printf("Sector Size: %dKB\n", f->sector_size);
            printf("Block Num: %d\n", f->block_num);
            printf("Sector Num: %d\n", f->sector_num);
        } else {
            printf("There have not a device named BY25Q64AS\n");
        }
        f++;
    }

    printf("***********BY25Q64AS page program test***********\n");
    printf("Write: BY25Q64AS len %d\n", strlen(FLASH_NAME));
    test_adr[0] = 0x0;
    test_adr[1] = 0x1;
    test_adr[2] = 0x0;

    by25q64as_write_enable();
    by25q64as_page_program(SPI_1, test_adr, FLASH_NAME, strlen(FLASH_NAME));
    printf("***********BY25Q64AS read data test***********\n");
    read_data = (unsigned char *)malloc(strlen(FLASH_NAME));
    memset(read_data, 0, strlen(FLASH_NAME));
    by25q64as_read_dara(SPI_1, test_adr, read_data, strlen(FLASH_NAME));
    printf("Read data: %s len %d\n", read_data, strlen(FLASH_NAME));
}
#endif
#endif // CONFIG_BY25Q64AS