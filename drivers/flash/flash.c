#include "flash.h"



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

#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS

static inline void spi_reuse_cfg(enum gpio_reg GPIO,
                                enum gpio_pin pin,
                                enum gpio_moder moder,
                                enum gpio_otype oty,
                                enum gpio_speed speed,
                                enum gpio_uppull pull)
{
    gpio_port_set(GPIO, pin, moder, oty, speed, pull);
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
            printf("----------------------------%s----------------------------\n", f->dev_name);
        } else {
            printf("There have not a device named BY25Q64AS\n");
        }
        f++;
    }
}
#endif
#endif // CONFIG_BY25Q64AS