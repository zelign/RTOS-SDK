# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

SPI_DIR := $(PRJ_ROOT_DIR)/drivers/spi
SPI_C_FILE := $(wildcard $(SPI_DIR)/*.c)
SPI_OBJ_FILE := $(BUILD_OUT)/spi.o
BUILD_SOURCE += $(SPI_OBJ_FILE)

all: $(SPI_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo "$(GREEN)Compile $(SPI_OBJ_FILE)$(END)"

$(SPI_OBJ_FILE): $(SPI_C_FILE)
	@ $(CC) $(DRV_FLAGS) -c $< -o $@