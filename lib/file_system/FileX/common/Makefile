# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
COMMON_SRC_DIR = $(PWD)/src

COMMON_SRC_C_FILE := $(wildcard $(COMMON_SRC_DIR)/*.c)
COMMON_SRC_OBJ = $(addprefix $(BUILD_OUT)/, $(notdir $(COMMON_SRC_C_FILE:.c=.o)))

all: $(COMMON_SRC_OBJ)

$(BUILD_OUT)/%.o: $(COMMON_SRC_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

.PHONY: all
