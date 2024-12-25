# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

SHELL := /bin/bash
PWD := $(shell pwd)
CLOCK_DIR := $(PWD)

CLOCK_C_FILE := $(wildcard $(CLOCK_DIR)/*.c)
CLOCK_OBJ_FILE := $(BUILD_OUT)/clock.o
BUILD_SOURCE += $(CLOCK_OBJ_FILE)

all: $(CLOCK_OBJ_FILE)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile ${notdir $(CLOCK_OBJ_FILE)} OK$(END)"
$(CLOCK_OBJ_FILE): $(CLOCK_C_FILE)
	@ $(CC) $(C_FLAGS) -c $< -o $@