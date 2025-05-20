# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
STM_USB_LIB_DIR = $(PWD)
STM_USB_DIR = 	$(STM_USB_LIB_DIR)/STM32_USB_Device_Library \
				# $(STM_USB_LIB_DIR)/USB_APP \
				# $(STM_USB_LIB_DIR)/USB_HAL

STM_USB_OBJ_DIR := $(BUILD_OUT)
STM_USB_C_FILE := $(wildcard $(STM_USB_DIR)/*.c)
STM_USB_C_OBJ = $(addprefix $(STM_USB_OBJ_DIR)/, $(notdir $(STM_USB_C_FILE:.c=.o)))
BUILD_SOURCE += $(STM_USB_C_OBJ)

all: $(STM_USB_C_OBJ)

$(STM_USB_C_OBJ)/%.o: $(STM_USB_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

.PHONY: all $(STM_USB_C_OBJ)