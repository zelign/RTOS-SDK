# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

LED_DIR += $(PRJ_ROOT_DIR)/drivers/led

LED_C_FILE := $(wildcard $(LED_DIR)/*.c)
LED_OBJ_FILE := $(BUILD_OUT)/led.o
BUILD_SOURCE += $(LED_OBJ_FILE)

all: $(LED_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo "$(GREEN)Compile $(LED_OBJ_FILE)$(END)"

$(LED_OBJ_FILE): $(LED_C_FILE)
	@ $(CC) $(DRV_FLAGS) -c $< -o $@