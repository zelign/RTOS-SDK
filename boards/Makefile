include $(BUILD_OUT)/config.mk

SHELL := /bin/bash
PWD := $(shell pwd)
BOARD_DIR_SIGNED := $(shell basename $(PWD))
BOARD_DIR := $(PRJ_ROOT_DIR)/boards
BOARD_BUILD_DIR := $(BUILD_OUT)

include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk

#define input and potput file
BOARD_SRC_FILE := $(wildcard $(BOARD_DIR)/$(ARCHS)/$(BOARDS)/*.c)
BOARD_C_OBJ := $(addprefix $(BOARD_BUILD_DIR)/, $(notdir $(BOARD_SRC_FILE:.c=.o)))

# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

#define compiler related
C_DEFS :=
C_INCLUDES := -I$(BOARD_DIR)/$(ARCHS)/$(BOARDS)/$(MCUS) \
			  -I$(BOARD_DIR)/$(ARCHS)/$(BOARDS) \
			  -I$(PRJ_ROOT_DIR)/lib/include \
			  -I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/include \
			  -I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/cortex-M4 \
			  -I$(PRJ_ROOT_DIR)/drivers/led \
			  -I$(PRJ_ROOT_DIR)/drivers/gpio \

C_FLAGS += $(MCU_PARA) $(C_DEFS) $(C_INCLUDES) -Og -Wall -fdata-sections -ffunction-sections
BUILD_SOURCE += $(BOARD_C_OBJ)

all: info $(BOARD_C_OBJ)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile $(BOARD_C_OBJ)$(END)"
	@ echo -e "	$(CYAN)Compile OK!$(END)"
info:
	@ echo -e "$(YELLOW)$(BOLD)[6] INTO $(UNDERLINE)$(BOARD_DIR_SIGNED)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"
$(BOARD_BUILD_DIR)/%.o: $(BOARD_DIR)/$(ARCHS)/$(BOARDS)/%.c
	@ $(CC) $(C_FLAGS) -c $< -o $@