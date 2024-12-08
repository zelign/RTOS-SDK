#include "flash.h"
#include <string.h>
#include <stdlib.h>

static struct flash_info flash_inf[] = {
#ifdef CONFIG_BY25Q64AS
    {"BY25Q64AS", 0x6816, 8192, 64, 4, 16},
#endif
};

static struct flash_partition partitions[] = {
#ifdef CONFIG_BY25Q64AS
    {
        .name = "user",
        .offset = 0,
        .size = 0,
    },
    {
        .name = "manager_data",
        .offset = 0,
        .size = 0,
    },
#endif
};

static struct flash_info *current_flash = NULL;

#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS

#define SECTOR_SIZE 4096
static char flash_buf[SECTOR_SIZE] = {0};
static bool need_erase = FALSE, multi_program_flag = FALSE;

struct flash_info * get_current_flash(void)
{
    if (current_flash)
        return current_flash;
    return NULL;
}

static void by25q64as_write_enable(void)
{
    unsigned char write_enable_cmd = 0x06;
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (! spi_chk_buy(SPI_1))
        spi_trans(SPI_1, &write_enable_cmd); //get manufacture//get device id
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void by25q64as_read_dara(enum spi_dev sd, unsigned int address, char *data, unsigned int data_len)
{
    unsigned char read_data_cmd = 0x03;
    unsigned char write_cmd = 0xff;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (address & 0xff)
        sm_printf("[Warning]Please alignment the address to page band (0x100)%x\n", address);
    if (! spi_chk_buy(sd)) {
        spi_trans(sd, &read_data_cmd);
        read_data_cmd = (unsigned char)(address >> 16);
        spi_trans(sd, &read_data_cmd);
        read_data_cmd = (unsigned char)(address >> 8);
        spi_trans(sd, &read_data_cmd);
        read_data_cmd = (unsigned char)(address >> 0);
        spi_trans(sd, &read_data_cmd);

        for (unsigned int i = 0; i < data_len; i++) {
            *(data) = spi_trans(sd, &write_cmd);
            data++;
        }
            
    } else {
        sm_printf("busy!\n");
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void by25q64as_sector_erase(enum spi_dev sd, unsigned int address)
{
    unsigned char erase_cmd = 0x20, adr[3] = {0};

    address &= ~0x00000fff;
    adr[0] = (address >> 16) & 0xff;
    adr[1] = (address >> 8) & 0xff;
    adr[2] = (address >> 0) & 0xff;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);

    if (! spi_chk_buy(sd)) {
        spi_trans(sd, &erase_cmd);
        spi_trans(sd, &adr[0]);
        spi_trans(sd, &adr[1]);
        spi_trans(sd, &adr[2]);
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void by25q64as_chip_erase(enum spi_dev sd)
{
    unsigned char erase_cmd = 0x60;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);

    if (! spi_chk_buy(sd))
        spi_trans(sd, &erase_cmd);

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void by25q64as_write_page(enum spi_dev sd, unsigned int address, void *data, unsigned int data_len)
{
    unsigned char page_program_cmd = 0x02;
    unsigned int sector_offset = 0, sector_address = 0;
    char * flash_buf_p = flash_buf;
    char * data_p = data;
    unsigned int program_len_per = (current_flash->sector_size * 1024)/current_flash->page_num;

    sector_offset = (address & 0x00000fff);
    sector_address = address & ~0x00000fff;

    /* whether the address is page alignment */
    if (address & 0xff) {
        sm_printf("Please alignment the address to page band (0x100) %x\n", address);
        return;
    }

    /* for recursion */
    if (!multi_program_flag) {
        memset(flash_buf, 0, SECTOR_SIZE);
        by25q64as_read_dara(sd, sector_address, &flash_buf[0], SECTOR_SIZE);
    }

    if (data_len <= program_len_per) {
        for(unsigned int i = sector_offset; i < data_len; i++) {
            if (flash_buf[i] != 0xff)
                need_erase = TRUE;
            flash_buf[i] = data_p[i - sector_offset];
        }
        if (multi_program_flag)
            return;
    } else {
        unsigned int page_num = data_len / program_len_per;
        unsigned int multi_program_add = address;
        multi_program_flag = TRUE;

        for(unsigned int j = 0; j < page_num;
                j++, multi_program_add += program_len_per, data_p += program_len_per)
            by25q64as_write_page(sd,
                multi_program_add,
                data_p,
                program_len_per);
        if (data_len % program_len_per)
            by25q64as_write_page(sd,
                (multi_program_add + program_len_per),
                (data_p + program_len_per),
                (data_len % program_len_per));

        multi_program_flag = FALSE;
    }

    data_p = data;

    /* flash program */
    if (need_erase) {
        by25q64as_sector_erase(sd, sector_address);
        
        for (unsigned int i = 0; i < current_flash->page_num;
                i++, sector_address += program_len_per, flash_buf_p += program_len_per) {
            page_program_cmd = 0x02;
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
            PAGE_PROGRAM(sd, page_program_cmd, sector_address, flash_buf_p, program_len_per, spi_trans)
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
        }

        flash_buf_p = flash_buf;
        need_erase = FALSE;
    } else {
        if (data_len <= program_len_per) {
            page_program_cmd = 0x02;
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
            PAGE_PROGRAM(sd, page_program_cmd, address, data_p, data_len, spi_trans)
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
        } else {
            for (unsigned int i = 0; i < data_len / program_len_per;
                    i++, address += program_len_per, data_p += program_len_per) {
                page_program_cmd = 0x02;
                gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
                PAGE_PROGRAM(sd, page_program_cmd, address, data_p, program_len_per, spi_trans)
                gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
            }
            if (data_len % program_len_per) {
                page_program_cmd = 0x02;
                gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
                PAGE_PROGRAM(sd,
                    page_program_cmd,
                    (address + program_len_per),
                    (data_p + program_len_per),
                    (data_len % program_len_per),
                    spi_trans)
                gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
            }
        }
    }
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

static void get_by25q64as_device_id(enum spi_dev sd, unsigned int *dev_id)
{
    unsigned char get_id_cmd = 0x90;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (! spi_chk_buy(sd)) {
        spi_trans(sd, &get_id_cmd); //get manufacture
        get_id_cmd = 0;
        spi_trans(sd, &get_id_cmd);
        spi_trans(sd, &get_id_cmd);
        spi_trans(sd, &get_id_cmd); //get device id
        get_id_cmd = 0xff;
        *dev_id |= spi_trans(sd, &get_id_cmd);
        *dev_id <<= 8;
        *dev_id |= spi_trans(sd, &get_id_cmd);
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

struct flash_info * find_flash_by_id(enum spi_dev sd, unsigned int id)
{
    struct flash_info *info = &flash_inf[0];

    for (unsigned int i = 0; i < sizeof(flash_inf)/sizeof(struct flash_info);
        i++, info++) {
        if (id == info->dev_id) {
            sm_printf("---------------%s---------------\n", info->dev_name);
            sm_printf("Chip Size: %dKB\n", info->chip_size);
            sm_printf("Flash ID: 0X%X\n", id);
            sm_printf("Block Size: %dKB\n", info->block_size);
            sm_printf("Sector Size: %dKB\n", info->sector_size);
            sm_printf("Page Num per sector: %d\n", info->page_num);
            return info;
        }
    }
    sm_printf("can not find the flash device that id is %d\n", id);
    return NULL;
}

struct flash_partition * get_partition_by_name(char *name)
{
    struct flash_partition *part = &partitions[0];

    if (!name)
        return NULL;

    for (unsigned int i = 0;
            i < sizeof(partitions)/sizeof(struct flash_partition);
            i++, part++)
        if (!strncmp(name, part->name, strlen(name)))
            return part;
    sm_printf("can not find the partition that name is %s\n", name);
    return NULL;
}

int partition_init(struct flash_info *info)
{
    struct flash_partition *part = NULL;
    unsigned int msize = 0;
    if (!info)
        return -1;

    part = get_partition_by_name("manager_data");
    if (!part)
        return -1;
    part->size = (info->page_num * (info->chip_size/info->sector_size))/1024;
    part->offset = ((info->chip_size * 1024)) - part->size * 1024;
    msize = part->size;

    part = get_partition_by_name("user");
    if (!part)
        return -1;
    part->size = info->chip_size * 1024 - msize * 1024;

    sm_printf("*********%s partition table*********\n", info->dev_name);
    part = &partitions[0];
    for(unsigned char i = 0;
        ((i < sizeof(partitions)/sizeof(struct flash_partition)) && (part != NULL));
            part++, i++)
        sm_printf("[%s] 0x%x-%dKB\n", part->name, part->offset, part->size);

    return 0;
}

void by25q64as_flash_test(enum spi_dev sd, unsigned int write_addr)
{
    char *read_data = NULL;

    sm_printf("***********BY25Q64AS page program test 0x%x***********\n", write_addr);
    sm_printf("Write: BY25Q64AS in 0x%x len %d\n", write_addr, strlen(FLASH_NAME));

    by25q64as_write_enable();
    by25q64as_write_page(sd, write_addr, FLASH_NAME, strlen(FLASH_NAME));

    sm_printf("***********BY25Q64AS read data test***********\n");

    read_data = (char *)malloc(strlen(FLASH_NAME));
    memset(read_data, 0, strlen(FLASH_NAME));

    by25q64as_read_dara(sd, write_addr, read_data, strlen(FLASH_NAME));

    if (!strncmp(FLASH_NAME, read_data, strlen(FLASH_NAME)))
        sm_printf("Flash test success: %s\n\n", read_data);
    else
        sm_printf("Flash test failed: %s\n\n", read_data);
    free(read_data);
}

void by25q64as_init(enum spi_dev sd)
{
    unsigned int id = 0;
    unsigned int test_add = 0;

    get_by25q64as_device_id(sd, &id);

    current_flash = find_flash_by_id(sd, id);
    if (!current_flash) {
        sm_printf("by25q64as init failed!\n");
        return;
    }

    if (partition_init(current_flash)) {
        sm_printf("partition init failed!\n");
        return;
    }
    // ERASE_ALL(sd)
    for (unsigned int i = 0; i < 32; i++, test_add += 0x100)
        by25q64as_flash_test(sd, test_add);
    

}

void spi_1_by25q64as_init(void)
{
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
    by25q64as_init(SPI_1);
}
#endif
#endif // CONFIG_BY25Q64AS