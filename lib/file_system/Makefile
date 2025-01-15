# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
FS_DIR = $(PWD)
FS_SUB_DIR =

ifeq ($(CONFIG_FILEX), y)
FS_SUB_DIR += $(FS_DIR)/FileX
endif

all: $(FS_SUB_DIR)

$(FS_SUB_DIR):
	$(Q) $(MAKE) all -C $@

.PHONY: all $(FS_SUB_DIR)