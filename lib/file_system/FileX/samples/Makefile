# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
SAMPLES_DIR = $(PWD)

SAMPLES_C_FILE := $(wildcard $(SAMPLES_DIR)/*.c)
SAMPLES_OBJ = $(addprefix $(BUILD_OUT)/, $(notdir $(SAMPLES_C_FILE:.c=.o)))

all: $(SAMPLES_OBJ)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk

$(BUILD_OUT)/%.o: $(SAMPLES_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

.PHONY: all
