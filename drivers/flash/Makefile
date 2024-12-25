# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

FLASH_DIR := $(PRJ_ROOT_DIR)/drivers/flash
FLASH_C_FILE := $(wildcard $(FLASH_DIR)/*.c)
FLASH_OBJ_FILE := $(BUILD_OUT)/flash.o
BUILD_SOURCE += $(FLASH_OBJ_FILE)

all: $(FLASH_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo "$(GREEN)Compile ${notdir $(FLASH_OBJ_FILE)} OK$(END)"

$(FLASH_OBJ_FILE): $(FLASH_C_FILE)
	@ $(CC) $(C_FLAGS) -c $< -o $@