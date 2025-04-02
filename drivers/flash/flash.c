#include "flash.h"
#include <string.h>
#include <stdlib.h>
#include "initcall.h"

#ifdef CONFIG_FLASH_CMD
#include "FreeRTOS_CLI.h"
#endif

#ifdef FLASH_DBG
#define FLASH_DBG printf
#else
#define FLASH_DBG(fmt, ...) \
	do {                \
	} while (0)
#endif

static struct flash_info flash_inf[] = {
#ifdef CONFIG_BY25Q64AS
	{ "BY25Q64AS", 0x6816, 8192, 64, 4, 16 },
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
static void by25q64as_write_enable(void);

#ifdef CONFIG_SPI
#ifdef CONFIG_BY25Q64AS

#define WRITE_ENABLE by25q64as_write_enable();
#define WAIT_WIP by25q64as_wip_wait();
#define SECTOR_SIZE (current_flash->sector_size * 1024)
#define PAGE_NUM (current_flash->page_num)
#define BLOCK_SIZE (current_flash->block_size * 1024)
#define CHIP_SIZE (current_flash->chip_size * 1024)
#define PAGE_LEN (SECTOR_SIZE / PAGE_NUM)

static char *flash_buffer = NULL;
static bool need_erase = FALSE, accross_sector_flag = FALSE,
	    flash_init_flag = FALSE;

bool is_flash_init(void)
{
	return flash_init_flag;
}

struct flash_info *get_current_flash(void)
{
	if (current_flash)
		return current_flash;
	return NULL;
}

static void by25q64as_write_enable(void)
{
	unsigned char write_enable_cmd = CS_CMD;
	FLASH_CS_LOW
	if (!spi_chk_busy(SPI_1))
		spi_trans(SPI_1, &write_enable_cmd);
	FLASH_CS_HIGH
}

static void by25q64as_wip_wait(void)
{
	unsigned char cmd = READ_STATUS_REGISTER_1_CMD;
	unsigned char reg = 1;
	while ((reg & 0x01)) {
		FLASH_CS_LOW
		if (!spi_chk_busy(SPI_1)) {
			spi_trans(SPI_1, &cmd);
			cmd = 0xff;
			reg = spi_trans(SPI_1, &cmd);
		}
		FLASH_CS_HIGH
		cmd = READ_STATUS_REGISTER_1_CMD;
	}
}

void by25q64as_read_data(enum spi_dev sd, unsigned int address, char *data,
			 unsigned int data_len)
{
	unsigned char read_data_cmd = READ_DATA_CMD;
	unsigned char cmd[3] = { (address >> 16) & 0xff, (address >> 8) & 0xff,
				 address & 0xff };
	unsigned char write_cmd = 0xff;
	char *data_ptr = data;

	FLASH_CS_LOW
	if (address & 0xff)
		printf("[Warning] Please align the address to page boundary (0x100) %x\n",
		       address);
	if (!spi_chk_busy(sd)) {
		spi_trans(sd, &read_data_cmd);
		spi_trans(sd, &cmd[0]);
		spi_trans(sd, &cmd[1]);
		spi_trans(sd, &cmd[2]);
		for (unsigned int i = 0; i < data_len; i++) {
			*data_ptr = spi_trans(sd, &write_cmd);
			data_ptr++;
		}
	} else {
		printf("busy!\n");
	}
	FLASH_CS_HIGH
}

void by25q64as_sector_erase(enum spi_dev sd, unsigned int address)
{
	unsigned char erase_cmd = 0x20;
	unsigned char adr[3] = { (address >> 16) & 0xff, (address >> 8) & 0xff,
				 address & 0xff };
	WRITE_ENABLE
	FLASH_CS_LOW
	if (!spi_chk_busy(sd)) {
		spi_trans(sd, &erase_cmd);
		spi_trans(sd, &adr[0]);
		spi_trans(sd, &adr[1]);
		spi_trans(sd, &adr[2]);
	}
	FLASH_CS_HIGH
	WAIT_WIP
}

void by25q64as_chip_erase(enum spi_dev sd)
{
	unsigned char erase_cmd = 0xc7; //0x60;

	while (spi_chk_busy(sd))
		;
	printf("Erase all chip ... \n");
	WRITE_ENABLE
	FLASH_CS_LOW
	spi_trans(sd, &erase_cmd);
	FLASH_CS_HIGH
}
/**
 * @brief Write data to flash and support for multi-page programming
 * 
 * @param sd SPI Device
 * @param address flash address will be programmed
 * @param data data will programming
 * @param data_len data length
 * @return int 0:success
 */
int by25q64as_read_write(enum spi_dev sd, unsigned int address, void *data,
			 unsigned int data_len)
{
	unsigned char cmd = PAGE_PROGRAM_CMD;
	char *flash_buffer_ptr = NULL;
	char *_data_ptr = NULL;
	unsigned int bytes_per_page = 0;
	unsigned int current_sector_num = 0;
	unsigned int _data_len = data_len, _address = address;
	unsigned int _programmed_len = 0;

	/* page alignment detection */
	if (_address & 0xff) {
		printf("Please align the address to page boundary (0x100) %x\n",
		       _address);
		return -1;
	}

	// length detection
	if (_address + _data_len > CHIP_SIZE) {
		printf("The data length is too long!\n");
		return -1;
	}

	if ((SECTOR_SIZE != 0) && (PAGE_NUM != 0)) {
		bytes_per_page = SECTOR_SIZE / PAGE_NUM;
	} else {
		printf("The sector size or page number is not correct!\n");
		return -1;
	}

	if (flash_buffer != NULL) {
		flash_buffer_ptr = flash_buffer;
	} else {
		printf("The flash buffer is NULL!\n");
		return -1;
	}

	if (data != NULL) {
		_data_ptr = data;
	} else {
		printf("The data is NULL!\n");
		return -1;
	}
	while (1) {
		/* signal page programming */
		if (_data_len <= bytes_per_page) {
			/* Read one page data from _address to flash buffer */
			by25q64as_read_data(sd, _address, flash_buffer_ptr,
					    bytes_per_page);
			FLASH_DBG("Completion of reading a page \n");

			/* Check whether the page is empty */
			for (unsigned int offset = 0; offset < bytes_per_page;
			     offset++) {
				if (flash_buffer_ptr[offset] != 0xff) {
					need_erase =
						TRUE; //The page is not empty, its needs to be erased before it can be programmed
					FLASH_DBG(
						"This page is not empty, need to erase \n");
					break;
				}
			}

			/* The page is empty, program directly */
			if (!need_erase) {
				FLASH_DBG(
					"This page is empty, program directly [%x-%x] \n",
					_data_ptr, _address);
				WRITE_ENABLE
				FLASH_CS_LOW
				PAGE_PROGRAM(sd, cmd, _address, _data_ptr,
					     _data_len, spi_trans);
				FLASH_CS_HIGH
				WAIT_WIP
				break;
			}
			/* mutiple page program */
		} else {
			/* Read a sector data to RAM(flash buffer) */
			by25q64as_read_data(sd, (_address & ~0xfff),
					    flash_buffer_ptr, SECTOR_SIZE);

			/* Confirm whether the position range that needs programming within the sector is empty */
			for (unsigned int offset = (_address & 0xfff);
			     offset < SECTOR_SIZE; offset++) {
				if (flash_buffer_ptr[offset] != 0xff) {
					need_erase = TRUE;
					break;
				}
			}

			/**
             * @brief confirm that whether need set mutiple program flag
             * (_address & ~0xfff) current sector number of _address.
             * ((_address + _data_len) & ~0xfff) next sector number of _address + _data_len, if the value larger than current sector
             * it's means need multiple page programming
             */
			if ((_address & ~0xfff) !=
			    ((_address + (data_len - _programmed_len)) &
			     ~0xfff)) {
				accross_sector_flag = TRUE;
				current_sector_num =
					(_address & ~0xfff) / SECTOR_SIZE;
			}

			/* If don't need erase, program directly */
			if (!need_erase) {
				/* Which page number is the address within a sector located in?  */
				unsigned int page_num =
					(_address & 0xfff) / bytes_per_page;
				_data_ptr = data;
				/* Program the data directly into flash, one page at a time */
				for (; page_num < PAGE_NUM; page_num++) {
					cmd = PAGE_PROGRAM_CMD;
					WRITE_ENABLE
					FLASH_CS_LOW
					PAGE_PROGRAM(
						sd, cmd,
						((_address & ~0xfff) +
						 page_num * bytes_per_page),
						_data_ptr, bytes_per_page,
						spi_trans);
					FLASH_CS_HIGH
					WAIT_WIP
					_data_ptr += bytes_per_page;
				}
				break;
			}
		}

		/* If erasing is needed, it means that at least one sector must be read, erased and writted */
		if (need_erase) {
			need_erase = FALSE;

			/* read - erase - write */

			/* read a sector size data to flash buffer */
			memset(flash_buffer_ptr, 0, SECTOR_SIZE);
			by25q64as_read_data(sd, (_address & ~0xfff),
					    flash_buffer_ptr, SECTOR_SIZE);

			/* write data into RAM */
			_data_ptr = data;
			for (unsigned int i = (_address & 0xfff);
			     (i < SECTOR_SIZE) &&
			     (i < ((_address & 0xfff) + _data_len));
			     i++)
				flash_buffer_ptr[i] = *_data_ptr++;

			/* erase a sector */
			by25q64as_sector_erase(sd, (_address & ~0xfff));

			FLASH_DBG("Completion of erasing a sector\n");

			for (unsigned int page_num = 0; page_num < PAGE_NUM;
			     page_num++) {
				cmd = PAGE_PROGRAM_CMD;
				WRITE_ENABLE
				FLASH_CS_LOW
				PAGE_PROGRAM(
					sd, cmd,
					((_address & ~0xfff) +
					 page_num *
						 bytes_per_page), /* write to */
					(flash_buffer_ptr +
					 (page_num *
					  bytes_per_page)), /* data from */
					bytes_per_page, spi_trans);
				FLASH_CS_HIGH
				WAIT_WIP
				FLASH_DBG("page_num %d\n", page_num);
			}
		}

		/* if need accross-sector program, that means you need program the next sector */
		if (accross_sector_flag) {
			accross_sector_flag = FALSE;

			/* update the critical parameters */

			/* Calculate the length has been programmed */
			_programmed_len =
				(((current_sector_num + 1) * SECTOR_SIZE) -
				 (address & ~0xFFF) - (address & 0xFFF));

			/* Completion of programming all the data */
			if (_programmed_len == data_len)
				break;

			/* Update the source data pointer */
			_data_ptr = data + _programmed_len;

			/* Update the next programming address within flash */
			_address = (current_sector_num + 1) * SECTOR_SIZE;

			/* Calculate the length for the next programming */
			_data_len = (data_len - _programmed_len < SECTOR_SIZE) ?
					    (data_len - _programmed_len) :
					    (SECTOR_SIZE);

			continue;
		}
		break;
	}
	return 0;
}

static void by25q64as_gpio_init(void)
{
	gpio_init(SPI1_GPIO, FLASH_CS_Pin, OUTPUT, PUSH, GPIO_50MHZ, UP_PULL,
		  SET);
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

	FLASH_CS_LOW
	if (!spi_chk_busy(sd)) {
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
	FLASH_CS_HIGH
}

struct flash_info *find_flash_by_id(enum spi_dev sd, unsigned int id)
{
	struct flash_info *info = &flash_inf[0];

	for (unsigned int i = 0;
	     i < sizeof(flash_inf) / sizeof(struct flash_info); i++, info++) {
		if (id == info->dev_id) {
			printf("---------------%s---------------\n",
			       info->dev_name);
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

struct flash_partition *get_partition_by_name(char *name)
{
	struct flash_partition *part = &partitions[0];

	if (!name)
		return NULL;

	for (unsigned int i = 0;
	     i < sizeof(partitions) / sizeof(struct flash_partition);
	     i++, part++)
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
	part->size =
		(info->page_num * (info->chip_size / info->sector_size)) / 1024;
	part->offset = ((info->chip_size * 1024)) - part->size * 1024;
	msize = part->size;

	part = get_partition_by_name("user");
	if (!part)
		return -1;
	part->size = info->chip_size * 1024 - msize * 1024;

	printf("*********%s partition table*********\n", info->dev_name);
	part = &partitions[0];
	for (unsigned char i = 0;
	     i < sizeof(partitions) / sizeof(struct flash_partition) &&
	     part != NULL;
	     part++, i++)
		printf("[%s] 0x%x-%dKB\n", part->name, part->offset,
		       part->size);

	return 0;
}

void by25q64as_flash_test(enum spi_dev sd, unsigned int write_addr)
{
	char *read_data = NULL;

	printf("***********BY25Q64AS page program test 0x%x***********\n",
	       write_addr);
	printf("Write: BY25Q64AS in 0x%x len %d\n", write_addr,
	       strlen(FLASH_NAME));

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
	spi_init(SPI_1, double_line_single_direct, send_rev_select_8_bit,
		 enable_software_management, first_msb, pclk_divide_256,
		 master_mode, motorola_mode, free_lower_first_edge_sample,
		 TRUE);
	by25q64as_init(SPI_1);
}

#ifdef CONFIG_FLASH_CMD

static void hexdump_flash_memort(unsigned int start_addr, unsigned int length,
				 unsigned int offset)
{
	unsigned char *ptr = (unsigned char *)start_addr;
	unsigned int i;

	for (i = 0; i < length; i++) {
		if (i % 16 == 0) {
			if (i != 0) {
				printf("  ");
				for (int j = i - 16; j < i; j++) {
					unsigned char c = ptr[j];
					if (c >= 32 && c <= 126)
						putc_usart1(c);
					else
						printf(".");
				}
			}
			printf("\n");
			printf("0x%x: ", (unsigned int)(offset + i));
		}
		printf("%X ", ptr[i]);
	}

	if (i % 16 != 0) {
		int padding = 16 - (i % 16);
		for (int j = 0; j < padding; j++) {
			printf("   ");
		}

		printf("  ");
		for (int j = i - (i % 16); j < i; j++) {
			unsigned char c = ptr[j];
			if (c >= 32 && c <= 126)
				putc_usart1(c);
			else
				printf(".");
		}
		printf("\n");
	}
}

/**
 * @brief store the sub command parameters
 * [0]: sub command
 * [1]-[3]: parameters
 */
static char *flash_sub_cmd_para[4] = { NULL };

static BaseType_t flash_init_command_callback(char *pcWriteBuffer,
					      size_t xWriteBufferLen,
					      const char *pcCommandString)
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
static BaseType_t flash_erase_command_callback(char *pcWriteBuffer,
					       size_t xWriteBufferLen,
					       const char *pcCommandString)
{
	unsigned int offset, len;

	offset = atoi(flash_sub_cmd_para[1]);
	len = atoi(flash_sub_cmd_para[2]);

	for (unsigned int i = 0; i < len;
	     i += SECTOR_SIZE, offset += SECTOR_SIZE)
		by25q64as_sector_erase(SPI_1, offset);

	return pdTRUE;
}

static BaseType_t flash_erase_all_command_callback(char *pcWriteBuffer,
						   size_t xWriteBufferLen,
						   const char *pcCommandString)
{
	printf("Erase all chip ... \n");
	by25q64as_chip_erase(SPI_1);
	return pdTRUE;
}

static BaseType_t flash_read_command_callback(char *pcWriteBuffer,
					      size_t xWriteBufferLen,
					      const char *pcCommandString)
{
	unsigned int address, len, offset;

	offset = atoi(flash_sub_cmd_para[1]);
	address = atoi(flash_sub_cmd_para[2]);
	len = atoi(flash_sub_cmd_para[3]);

	by25q64as_read_data(SPI_1, offset, (char *)address, len);
	return pdTRUE;
}

static BaseType_t flash_hexdump_command_callback(char *pcWriteBuffer,
						 size_t xWriteBufferLen,
						 const char *pcCommandString)
{
	unsigned int *address, len, offset;

	offset = atoi(flash_sub_cmd_para[1]);
	len = atoi(flash_sub_cmd_para[2]);

	address = (unsigned int *)malloc(len);

	by25q64as_read_data(SPI_1, offset, (char *)address, len);
	hexdump_flash_memort((unsigned int)address, len, offset);
	free(address);
	return pdTRUE;
}

static BaseType_t flash_write_command_callback(char *pcWriteBuffer,
					       size_t xWriteBufferLen,
					       const char *pcCommandString)
{
	printf("flash write\n");
	unsigned int address, len, offset;

	offset = atoi(flash_sub_cmd_para[1]);
	address = atoi(flash_sub_cmd_para[2]);
	len = atoi(flash_sub_cmd_para[3]);

	by25q64as_read_write(SPI_1, offset, (void *)address, len);
	return pdTRUE;
}

static BaseType_t flash_test_command_callback(char *pcWriteBuffer,
					      size_t xWriteBufferLen,
					      const char *pcCommandString)
{
	by25q64as_flash_test(SPI_1, 0);
	return pdTRUE;
}

static BaseType_t flash_help_command_callback(char *pcWriteBuffer,
					      size_t xWriteBufferLen,
					      const char *pcCommandString)
{
	printf("\r\nhelp:\r\n");
	printf("flash init:                             Initialize the flash\n");
	printf("flash erase [address] [len]:            erase the flash from [address] with [len]\n");
	printf("flash read [offset] [address] [len]:    read the flash data from [offset] into [address] in RAM with [len]\n");
	printf("flash hexdump [offset] [len]:           read the flash data from [offset] with [len] and print\n");
	printf("flash write [offset] [address] [len]:   write the data from [address] in RAM into flash [offset] position with [len]\n");
	printf("flash test [start address] [len]:       test the flash with addresss range is [start address] to [start address] + len\n");
	printf("\t\tThe default start address is 0x0 and the default len is 1MB\n");
	printf("flash help: Lists all the registered commands\r\n\r\n");
	return pdTRUE;
}

static CLI_Command_Definition_t flash_sub_command[] = {
	{ "init", NULL, flash_init_command_callback, -1, NULL },
	{ "erase", NULL, flash_erase_command_callback, -1, NULL },
	{ "erase_all", NULL, flash_erase_all_command_callback, -1, NULL },
	{ "read", NULL, flash_read_command_callback, -1, NULL },
	{ "hexdump", NULL, flash_hexdump_command_callback, -1, NULL },
	{ "write", NULL, flash_write_command_callback, -1, NULL },
	{ "test", NULL, flash_test_command_callback, -1, NULL },
	{ "help", NULL, flash_help_command_callback, -1, NULL }
};

/**
 * @brief 
 * 
 * @param pcWriteBuffer @ pointer to the first subcommand or terminal '\0'
 * @param xWriteBufferLen @ the length of the pcWriteBuffer
 * @param pcCommandString @ the command string
 * @return BaseType_t 
 */
static BaseType_t flash_command_callback(char *pcWriteBuffer,
					 size_t xWriteBufferLen,
					 const char *pcCommandString)
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
				flash_sub_cmd_para[sub_cmd_index] =
					(char *)malloc(
						sub_cmd_len +
						1); //sub command length + '\0'

				if (!flash_sub_cmd_para[sub_cmd_index]) {
					printf("malloc sub command failed! %d\n",
					       __LINE__);
					return pdFALSE;
				}

				strncpy(flash_sub_cmd_para[sub_cmd_index],
					start, sub_cmd_len);
				flash_sub_cmd_para[sub_cmd_index][sub_cmd_len] =
					'\0';
				sub_cmd_index++;
				start = end + 1;
			}
		}

		/* confirm that has one or two subcommand */
		if ((*end == '\0') && (start < end)) { //the last sub command
			if (start == pcWriteBuffer + 1) { //only one sub command
				sub_cmd_index = 0;
				flash_sub_cmd_para[sub_cmd_index] =
					(char *)malloc(strlen(start) + 1);
			} else { //more than one sub command
				flash_sub_cmd_para[sub_cmd_index] =
					(char *)malloc(strlen(start) + 1);
			}
			if (!flash_sub_cmd_para[sub_cmd_index]) {
				printf("malloc sub command failed!\n");
				return pdFALSE;
			}

			strncpy(flash_sub_cmd_para[sub_cmd_index], start,
				(unsigned int)(end - start));
			flash_sub_cmd_para[sub_cmd_index][strlen(start)] =
				'\0'; // Ensure null-termination
		}

		/* execute sub commands */
		CLI_Command_Definition_t *flash_subcmd = NULL;
		CLI_Definition_List_Item_t *flash_subcmd_list = NULL;
		for (flash_subcmd_list = get_registered_commands();
		     flash_subcmd_list != NULL;
		     flash_subcmd_list = flash_subcmd_list->pxNext)
			if (!strncmp(pcCommandString,
				     flash_subcmd_list->pxCommandLineDefinition
					     ->pcCommand,
				     strlen(pcCommandString)))
				break;

		/* find the matched sub command */
		if ((flash_subcmd_list != NULL) &&
		    (flash_subcmd_list->pxCommandLineDefinition->subcmd !=
		     NULL)) {
			flash_subcmd =
				flash_subcmd_list->pxCommandLineDefinition
					->subcmd;
			for (unsigned char num = 0;
			     num < sizeof(flash_sub_command) /
					   sizeof(CLI_Command_Definition_t);
			     num++, flash_subcmd++) {
				if (!strncmp(
					    flash_subcmd->pcCommand,
					    flash_sub_cmd_para[0],
					    ((strlen(flash_subcmd->pcCommand) >
					      strlen(flash_sub_cmd_para[0])) ?
						     strlen(flash_subcmd
								    ->pcCommand) :
						     strlen(flash_sub_cmd_para
								    [0])))) {
					if (flash_subcmd->pxCommandInterpreter) {
						flash_subcmd
							->pxCommandInterpreter(
								NULL, 0, NULL);
						return pdTRUE;
					}
				}
			}
		}
	}
	printf("Please input correct command:\n");
	flash_help_command_callback(NULL, 0, NULL);
	return pdTRUE;
}

static const CLI_Command_Definition_t flash_command = {
	"flash", NULL, flash_command_callback, -1, &flash_sub_command[0],
};

static void flash_cmd_init()
{
	FreeRTOS_CLIRegisterCommand(&flash_command);
}

APP_INIT_1(flash_cmd_init);
#endif // CONFIG_FLASH_CMD

#endif
#endif // CONFIG_BY25Q64AS
