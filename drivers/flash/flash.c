#include "flash.h"
#include <string.h>
#include <stdlib.h>
#include "initcall.h"

#ifdef CONFIG_FLASH_CMD
#include "FreeRTOS_CLI.h"
#endif

#ifndef FLASH_DBG
#define FLASH_DBG printf
#endif

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

#define SECTOR_SIZE (current_flash->sector_size * 1024)
#define PAGE_NUM    (current_flash->page_num)
#define BLOCK_SIZE  (current_flash->block_size * 1024)
#define CHIP_SIZE   (current_flash->chip_size * 1024)
#define PAGE_LEN    (SECTOR_SIZE / PAGE_NUM)

static char *flash_buffer = NULL;
static bool need_erase = FALSE, multi_program_flag = FALSE, flash_init_flag = FALSE;

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
    if (!spi_chk_buy(SPI_1))
        spi_trans(SPI_1, &write_enable_cmd);
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void by25q64as_read_data(enum spi_dev sd, unsigned int address, char *data, unsigned int data_len)
{
    unsigned char read_data_cmd = READ_DATA_CMD;
    unsigned char write_cmd = 0xff;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (address & 0xff)
        printf("[Warning] Please align the address to page boundary (0x100) %x\n", address);
    if (!spi_chk_buy(sd)) {
        spi_trans(sd, &read_data_cmd);
        read_data_cmd = (unsigned char)(address >> 16);
        spi_trans(sd, &read_data_cmd);
        read_data_cmd = (unsigned char)(address >> 8);
        spi_trans(sd, &read_data_cmd);
        read_data_cmd = (unsigned char)(address);
        spi_trans(sd, &read_data_cmd);

        for (unsigned int i = 0; i < data_len; i++) {
            *data = spi_trans(sd, &write_cmd);
            data++;
        }
    } else {
        printf("busy!\n");
    }
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

void by25q64as_sector_erase(enum spi_dev sd, unsigned int address)
{
    unsigned char erase_cmd = 0x20;
    unsigned char adr[3] = { (address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff };

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (!spi_chk_buy(sd)) {
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
    if (!spi_chk_buy(sd))
        spi_trans(sd, &erase_cmd);
    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
}

int by25q64as_read_write(enum spi_dev sd, unsigned int address, void *data, unsigned int data_len)
{
    unsigned char page_program_cmd = PAGE_PROGRAM_CMD;
    char *flash_buf_p = NULL;
    char *data_p = NULL;
    unsigned int program_len_per = 0;
    char *read_flash_data = NULL;

    if (address & 0xff) {
        printf("Please align the address to page boundary (0x100) %x\n", address);
        return -1;
    }
    // length detection
    if (address + data_len > CHIP_SIZE) {
        printf("The data length is too long!\n");
        return -1;
    }

    if ((SECTOR_SIZE != 0) && (PAGE_NUM != 0)) {
        program_len_per = SECTOR_SIZE / PAGE_NUM;
        FLASH_DBG("program_len_per: %d\n", program_len_per);
    } else {
        printf("The sector size or page number is not correct!\n");
        return -1;
    }

    if (flash_buffer != NULL) {
        flash_buf_p = flash_buffer;
    } else {
        printf("The flash buffer is NULL!\n");
        return -1;
    }

    if (data != NULL) {
        data_p = data;
    } else {
        printf("The data is NULL!\n");
        return -1;
    }

    if (data_len <= program_len_per) { //one page program

        read_flash_data = (char *)malloc(program_len_per);
        if(!read_flash_data) {
            printf("malloc read_flash_data failed!\n");
            return -1;
        }

        memset(read_flash_data, 0, program_len_per);
        FLASH_DBG("start read data\n");
        by25q64as_read_data(sd, (address & ~0xff), read_flash_data, program_len_per);

        unsigned char offset = 0;

        for (offset = 0; offset < program_len_per; offset++)
            if (read_flash_data[offset] != 0xff) {
                need_erase = TRUE;
                FLASH_DBG("erase flag\n");
                break;
            }

        free(read_flash_data);
        FLASH_DBG("offset: %d\n", offset);
        if (offset == program_len_per) {//The page is empty, program directly
            FLASH_DBG("page program\n");
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
            PAGE_PROGRAM(sd, page_program_cmd, address, data_p, data_len, spi_trans);
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
        }
    /* mutiple page program */
    } else {
        // confirm that whether need set erase flag by read current sector data to RAM
        by25q64as_read_data(sd, (address & ~0xfff), flash_buf_p, SECTOR_SIZE);
        //I need traverse the flash buffer from the offset of the address to confirm whether need erase the sector
        unsigned int offset = 0; //get the offset of the address in the sector

        for (offset = (address & 0xfff); offset < SECTOR_SIZE; offset++)
            if (flash_buf_p[offset] != 0xff) { //confirm that the remain data in the sector is empty
                need_erase = TRUE; //the remaining space in the sector is not empty, need erase
                break;
            }

        // confirm that whether need set mutiple program flag
        if ((address & ~0xfff) != ((address + data_len) & ~0xfff))
            multi_program_flag = TRUE;  //at least over one sector, need mutiple program

        if (offset == SECTOR_SIZE) { //The sector is empty, program directly
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
            PAGE_PROGRAM(sd, page_program_cmd, address, data_p,
                (multi_program_flag?(SECTOR_SIZE - (address & 0xfff)):data_len), spi_trans);
            gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
        }
    }

    /* If erasing is needed, it means that at least one sector must be read, erased and writted */
    if (need_erase) {
        FLASH_DBG("need erase\n");
        /* write data into RAM */
        for (unsigned int i = (address & 0xfff); i < SECTOR_SIZE && i < data_len; i++)
            flash_buf_p[i] = *data_p++; //It well important that the data_p must be increased

        FLASH_DBG("start erase\n");
        // erase sector
        by25q64as_sector_erase(sd, (address & ~0xfff));
        FLASH_DBG("start program\n");
        // write the sector data into flash
        gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
        PAGE_PROGRAM(sd, page_program_cmd, (address & ~0xfff), flash_buf_p, SECTOR_SIZE, spi_trans);
        gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, SET);
        // clear the erase flag
        need_erase = FALSE;
    }

    if (multi_program_flag) { //if need mutiple program, that means you need program the next sector
        multi_program_flag = FALSE;
        by25q64as_read_write(sd, ((address + data_len) & ~0xfff), data_p, (data_len - (SECTOR_SIZE - (address & 0xfff))));
    }
    return 0;
}

static void by25q64as_gpio_init(void)
{
    gpio_init(SPI1_GPIO, FLASH_CS_Pin, OUTPUT, PUSH, GPIO_50MHZ, UP_PULL, SET);
    pin_afr(SPI1_GPIO, SPI1_SCK, AF5);
    pin_afr(SPI1_GPIO, SPI1_MISO, AF5);
    pin_afr(SPI1_GPIO, SPI1_MOSI, AF5);
    spi_reuse_cfg(SPI1_GPIO, SPI1_SCK, REUSE, PUSH, GPIO_50MHZ, UP_PULL);
    spi_reuse_cfg(SPI1_GPIO, SPI1_MISO, REUSE, PUSH, GPIO_50MHZ, UP_PULL);
    spi_reuse_cfg(SPI1_GPIO, SPI1_MOSI, REUSE, PUSH, GPIO_50MHZ, UP_PULL);
}

static void get_by25q64as_device_id(enum spi_dev sd, unsigned int *dev_id)
{
    unsigned char get_id_cmd = MANUFACTURER_CMD;

    gpio_pin_cfg(SPI1_GPIO, FLASH_CS_Pin, RESET);
    if (!spi_chk_buy(sd)) {
        spi_trans(sd, &get_id_cmd);
        get_id_cmd = 0;
        spi_trans(sd, &get_id_cmd);
        spi_trans(sd, &get_id_cmd);
        spi_trans(sd, &get_id_cmd);
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

    for (unsigned int i = 0; i < sizeof(flash_inf) / sizeof(struct flash_info); i++, info++) {
        if (id == info->dev_id) {
            printf("---------------%s---------------\n", info->dev_name);
            printf("Chip Size: %dKB\n", info->chip_size);
            printf("Flash ID: 0X%X\n", id);
            printf("Block Size: %dKB\n", info->block_size);
            printf("Sector Size: %dKB\n", info->sector_size);
            printf("Page Num per sector: %d\n", info->page_num);
            return info;
        }
    }
    printf("Cannot find the flash device with ID %d\n", id);
    return NULL;
}

struct flash_partition * get_partition_by_name(char *name)
{
    struct flash_partition *part = &partitions[0];

    if (!name)
        return NULL;

    for (unsigned int i = 0; i < sizeof(partitions) / sizeof(struct flash_partition); i++, part++)
        if (!strncmp(name, part->name, strlen(name)))
            return part;
    printf("Cannot find the partition with name %s\n", name);
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
    part->size = (info->page_num * (info->chip_size / info->sector_size)) / 1024;
    part->offset = ((info->chip_size * 1024)) - part->size * 1024;
    msize = part->size;

    part = get_partition_by_name("user");
    if (!part)
        return -1;
    part->size = info->chip_size * 1024 - msize * 1024;

    printf("*********%s partition table*********\n", info->dev_name);
    part = &partitions[0];
    for (unsigned char i = 0; i < sizeof(partitions) / sizeof(struct flash_partition) && part != NULL; part++, i++)
        printf("[%s] 0x%x-%dKB\n", part->name, part->offset, part->size);

    return 0;
}

void by25q64as_flash_test(enum spi_dev sd, unsigned int write_addr)
{
    char *read_data = NULL;

    printf("***********BY25Q64AS page program test 0x%x***********\n", write_addr);
    printf("Write: BY25Q64AS in 0x%x len %d\n", write_addr, strlen(FLASH_NAME));

    by25q64as_write_enable();
    by25q64as_read_write(sd, write_addr, FLASH_NAME, strlen(FLASH_NAME));

    printf("***********BY25Q64AS read data test***********\n");

    read_data = (char *)malloc(strlen(FLASH_NAME));
    memset(read_data, 0, strlen(FLASH_NAME));

    by25q64as_read_data(sd, write_addr, read_data, strlen(FLASH_NAME));

    if (!strncmp(FLASH_NAME, read_data, strlen(FLASH_NAME)))
        printf("Flash test success: %s\n\n", read_data);
    else
        printf("Flash test failed: %s\n\n", read_data);
    free(read_data);
}

void by25q64as_init(enum spi_dev sd)
{
    unsigned int id = 0;
    unsigned int test_add = 0;

    get_by25q64as_device_id(sd, &id);

    current_flash = find_flash_by_id(sd, id);
    if (!current_flash) {
        printf("by25q64as init failed!\n");
        return;
    }
    flash_buffer = (char *)malloc(SECTOR_SIZE);
    if (!flash_buffer) {
        printf("malloc flash buffer failed!\n");
        return;
    }
    memset(flash_buffer, 0, SECTOR_SIZE);

    if (partition_init(current_flash)) {
        printf("partition init failed!\n");
        return;
    }

    // for (unsigned int i = 0; i < 32; i++, test_add += 0x100)
    //     by25q64as_flash_test(sd, test_add);
    flash_init_flag = TRUE;
}

void spi_1_by25q64as_init(void)
{
    if (flash_init_flag) {
        printf("The flash has been initialized!\n");
        return;
    }

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

#ifdef CONFIG_FLASH_CMD

/**
 * @brief store the sub command parameters
 * [0]: sub command
 * [1]-[3]: parameters
 */
static char *flash_sub_cmd_para[4] = {NULL};

static BaseType_t flash_init_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    spi_1_by25q64as_init();
    return pdTRUE;
}

/**
 * @brief
 * 
 * @param pcWriteBuffer @ pointer to the first parameter
 * @param xWriteBufferLen 
 * @param pcCommandString 
 * @return BaseType_t 
 */
static BaseType_t flash_erase_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    printf("flash erase\n");
    return pdTRUE;
}

static BaseType_t flash_read_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    printf("flash read\n");
    return pdTRUE;
}

static BaseType_t flash_write_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    printf("flash write\n");
    return pdTRUE;
}

static BaseType_t flash_test_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    printf("flash test\n");
    return pdTRUE;
}

static BaseType_t flash_help_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    printf("\r\nhelp:\r\n");
    printf("flash init: Initialize the flash\n");
    printf("flash erase address len: erase the flash from address with len\n");
    printf("flash read offset address len: read the flash data from offset into address in RAM with len\n");
    printf("flash write offset address len: write the data from address in RAM into flash offset position with len\n");
    printf("flash test [start address] [len]: test the flash with addresss range is start address to start address + len\n");
    printf("\t\tThe default start address is 0x0 and the default len is 1MB\n");
    printf("flash help: Lists all the registered commands\r\n\r\n");
    return pdTRUE;
}

static CLI_Command_Definition_t flash_sub_command[] =
{
	{
        "init",
	    NULL,
	    flash_init_command_callback,
	    0,
        NULL
    },
    {
        "erase",
        NULL,
        flash_erase_command_callback,
        0,
        NULL
    },
    {
        "read",
        NULL,
        flash_read_command_callback,
        0,
        NULL
    },
    {
        "write",
        NULL,
        flash_write_command_callback,
        0,
        NULL
    },
    {
        "test",
        NULL,
        flash_test_command_callback,
        0,
        NULL
    },
    {
        "help",
        NULL,
        flash_help_command_callback,
        0,
        NULL
    }    
};

/**
 * @brief 
 * 
 * @param pcWriteBuffer @ pointer to the first subcommand or terminal '\0'
 * @param xWriteBufferLen @ the length of the pcWriteBuffer
 * @param pcCommandString @ the command string
 * @return BaseType_t 
 */
static BaseType_t flash_command_callback( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    if (!pcCommandString) { //confirm that whether the command has a sub command
        printf("pcCommandString is NULL!\n");
        return pdFALSE;
    }

    if (strncmp(pcCommandString, "flash", 5)) {
        printf("The command is not flash command!\n");
        return pdFALSE;
    }
    printf("\n");
    if (pcWriteBuffer) { //confirm that whether the command has sub command
        char *start = NULL, *end = NULL;
        unsigned char sub_cmd_index = 0;
        start = pcWriteBuffer;
        end = pcWriteBuffer;

        memset(flash_sub_cmd_para, 0, sizeof(flash_sub_cmd_para));
        /* get all sub commands and store them into flash_sub_cmd_para array */
        for (end = pcWriteBuffer; *end != '\0'; end++) {
            if (*end == 0x20) { //moore than one sub command
                if (start == end) {
                    start = end + 1;
                    continue;
                }
                unsigned char sub_cmd_len = end - start;
                flash_sub_cmd_para[sub_cmd_index] = (char *)malloc(sub_cmd_len + 1); //sub command length + '\0'

                if (!flash_sub_cmd_para[sub_cmd_index]) {
                    printf("malloc sub command failed! %d\n", __LINE__);
                    return pdFALSE;
                }

                strncpy(flash_sub_cmd_para[sub_cmd_index], start, sub_cmd_len);
                flash_sub_cmd_para[sub_cmd_index][sub_cmd_len] = '\0';
                sub_cmd_index++;
                start = end + 1;
            }
        }

        /* confirm that has one or two subcommand */
        if ((*end == '\0') && (start != end)) { //the last sub command
            if (start == pcWriteBuffer + 1) { //only one sub command
                sub_cmd_index = 0;
                flash_sub_cmd_para[sub_cmd_index] = (char *)malloc(strlen(start) + 1);   
            } else   //more than one sub command
                flash_sub_cmd_para[sub_cmd_index] = (char *)malloc(strlen(start) + 1);

            if (!flash_sub_cmd_para[sub_cmd_index]) {
                    printf("malloc sub command failed!\n");
                    return pdFALSE;
            }

            strncpy(flash_sub_cmd_para[sub_cmd_index], start, (strlen(start) + 1));
        }

        /* execute sub commands */
        CLI_Command_Definition_t *flash_subcmd = NULL;
        CLI_Definition_List_Item_t *flash_subcmd_list = NULL;
        for (flash_subcmd_list = get_registered_commands(); flash_subcmd_list != NULL; flash_subcmd_list = flash_subcmd_list->pxNext)
            if (!strncmp(pcCommandString, flash_subcmd_list->pxCommandLineDefinition->pcCommand, strlen(pcCommandString)))
                break;

        /* find the matched sub command */
        if ((flash_subcmd_list != NULL) && (flash_subcmd_list->pxCommandLineDefinition->subcmd != NULL)) {
            flash_subcmd = flash_subcmd_list->pxCommandLineDefinition->subcmd;
            for (unsigned char num = 0; num < sizeof(flash_sub_command)/sizeof(CLI_Command_Definition_t);
                num++, flash_subcmd++) {
                if (!strncmp(flash_subcmd->pcCommand, flash_sub_cmd_para[0], strlen(flash_sub_cmd_para[0])))
                    if (flash_subcmd->pxCommandInterpreter) {
                        flash_subcmd->pxCommandInterpreter(NULL, 0, NULL);
                        return pdTRUE;
                    }
            }
        }
    }
    printf("Please input correct command:\n");
    flash_help_command_callback(NULL, 0, NULL);
    return pdTRUE;
}

static const CLI_Command_Definition_t flash_command =
{
	"flash",
	NULL,
	flash_command_callback,
	0,
    &flash_sub_command[0],
};

static void flash_cmd_init()
{
    FreeRTOS_CLIRegisterCommand(&flash_command);
}

APP_INIT_1(flash_cmd_init);
#endif // CONFIG_FLASH_CMD

#endif
#endif // CONFIG_BY25Q64AS