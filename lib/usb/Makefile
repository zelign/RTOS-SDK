# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
USB_DIR = $(PWD)
USB_SUB_DIR =

ifeq ($(CONFIG_STM32_USB_LIB), y)
USB_SUB_DIR += $(USB_DIR)/$(subst ",,$(CONFIG_USB))
endif

all: $(USB_SUB_DIR)

$(USB_SUB_DIR):
	@ echo -e "	$(GREEN)$(subst ",,$(CONFIG_USB)) ...$(END)"
	$(Q) $(MAKE) all -C $@

.PHONY: all $(USB_SUB_DIR)