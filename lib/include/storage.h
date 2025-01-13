#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "flash.h"
#include "autoconfig.h"

#ifdef CONFIG_BY25Q64AS
#define flash_read(sd, address, data, data_len) by25q64as_read_data(sd, address, data, data_len)
#define flash_write(sd, address, data, data_len) by25q64as_read_write(sd, address, data, data_len)
#define flash_sector_erase(sd, address) by25q64as_sector_erase(sd, address)
#define flash_chip_erase(sd) by25q64as_chip_erase(sd)
#endif

#endif

