# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

GPIO_DIR := $(PRJ_ROOT_DIR)/drivers/gpio
GPIO_C_FILE := $(wildcard $(GPIO_DIR)/*.c)
GPIO_OBJ_FILE := $(BUILD_OUT)/gpio.o
BUILD_SOURCE += $(GPIO_OBJ_FILE)

all: $(GPIO_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo "$(GREEN)Compile $(GPIO_OBJ_FILE)$(END)"

$(GPIO_OBJ_FILE): $(GPIO_C_FILE)
	@ $(CC) $(DRV_FLAGS) -c $< -o $@