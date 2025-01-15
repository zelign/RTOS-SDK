#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "flash.h"
#include "autoconfig.h"

#ifdef CONFIG_BY25Q64AS
#define flash_storage_write(...) by25q64as_read_write(SPI_1, __VA_ARGS__)
#define flash_storage_read(...) by25q64as_read_data(SPI_1, __VA_ARGS__)
#define flash_sector_erase(...) by25q64as_sector_erase(SPI_1, __VA_ARGS__)
#define flash_chip_erase() by25q64as_chip_erase(SPI_1)
#endif

#endif

