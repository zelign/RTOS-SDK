# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

KEY_DIR += $(PRJ_ROOT_DIR)/drivers/key

KEY_C_FILE := $(wildcard $(KEY_DIR)/*.c)
KEY_OBJ_FILE := $(BUILD_OUT)/key.o
BUILD_SOURCE += $(KEY_OBJ_FILE)

all: $(KEY_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo "$(GREEN)Compile ${notdir $(KEY_OBJ_FILE)} OK$(END)"

$(KEY_OBJ_FILE): $(KEY_C_FILE)
	@ $(CC) $(DRV_FLAGS) -c $< -o $@