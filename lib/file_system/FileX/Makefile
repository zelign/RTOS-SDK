# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
FX_DIR = $(PWD)

FX_BUILD_DIR =	$(FX_DIR)/samples \
				$(FX_DIR)/common

C_FLAGS +=  -I$(FX_DIR)/common/inc \
			-I$(FX_DIR)/ports \
			-I$(FX_DIR)/ports/generic/inc \
			-I$(FX_DIR)/samples \

all: $(FX_BUILD_DIR)

$(FX_BUILD_DIR):
	$(Q) $(MAKE) all -C $@
.PHONY: all $(FX_BUILD_DIR)
