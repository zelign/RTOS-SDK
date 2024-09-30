# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

USART_DIR += $(PRJ_ROOT_DIR)/drivers/usart

USART_C_FILE := $(wildcard $(USART_DIR)/*.c)
USART_OBJ_FILE := $(BUILD_OUT)/usart.o
BUILD_SOURCE += $(USART_OBJ_FILE)

all: $(USART_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo "$(GREEN)Compile $(USART_OBJ_FILE)$(END)"

$(USART_OBJ_FILE): $(USART_C_FILE)
	@ $(CC) $(DRV_FLAGS) -c $< -o $@