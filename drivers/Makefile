# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(PRJ_ROOT_DIR)/output/.config
SHELL := /bin/bash
PWD := $(shell pwd)
DRV_DIR_SINGLE := $(shell basename $(PWD))
DRV_DIR = clock usart gpio

ifeq ($(CONFIG_KEY), y)
DRV_DIR += key
endif

ifeq ($(CONFIG_LED), y)
DRV_DIR += led
endif

ifeq ($(CONFIG_SPI), y)
DRV_DIR += spi
endif

ifeq ($(or $(CONFIG_NOR_FLASH), $(CONFIG_NAND_FLASH)),y)
DRV_DIR += flash
endif

$(DRV_DIR):
	$(Q) $(MAKE) all -C $@

all: info $(DRV_DIR)

info:
	$(Q) echo -e "$(YELLOW)$(BOLD)[5] INTO $(UNDERLINE)$(DRV_DIR_SINGLE)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"

.PHONY: $(DRV_DIR)