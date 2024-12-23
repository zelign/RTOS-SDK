# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

#This Makefile will receive environment variables like follow:
#PRJ_ROOT_DIR: defined by top level Makefile, it's value is root directory.
#ARCHS: defined by build.sh
#PRODUCTS
#BUILD_OUT: output/xx-xx-xx-xx/build

#This makefile will be called by the top level Makefile
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk

ifeq ($(USE_KCONFIG), yes)
include $(PRJ_ROOT_DIR)/output/.config
else
include $(BUILD_OUT)/config.mk
ARCH_CORE :=
endif

SHELL := /bin/bash
PWD := $(shell pwd)
ARCH_DIR := $(PWD)
#This variable is useful to obtain the directory named arch
ARCH_DIR_SINGLE := $(shell basename $(ARCH_DIR))
Q := @

#define compiler related parameters
C_DEFS =

#define where to find the header files, it will increased as needed in future.
C_INCLUDES = -I$(PRJ_ROOT_DIR)/lib/include \
			-I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS)/$(MCUS) \
			-I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS) \
			-I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/$(ARCH_CORE) \
			-I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/include \
			-I$(PRJ_ROOT_DIR)/drivers/clock \
			-I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/include \
			-I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/portable/GCC/ARM_CM4F

C_FLAGS += $(MCU_PARA) $(C_DEFS) $(C_INCLUDES) -Og -Wall -fdata-sections -ffunction-sections

#define object file related variable
ARCH_ASM_FILE := $(wildcard $(ARCH_DIR)/$(ARCHS)/*.s)
ARCH_C_FILE := $(wildcard $(ARCH_DIR)/$(ARCHS)/*.c)
ARCH_OBJ_DIR := $(BUILD_OUT)

ARCH_OBJ = $(addprefix $(ARCH_OBJ_DIR)/, $(notdir $(ARCH_C_FILE:.c=.o)))

#define obj file path
ARCH_ASM_OBJ := $(addprefix $(ARCH_OBJ_DIR)/, $(notdir $(ARCH_ASM_FILE:.s=.o)))
BUILD_SOURCE += $(ARCH_ASM_OBJ)
BUILD_SOURCE += $(ARCH_OBJ)


$(warning "MCU_PARA $(MCU_PARA)")
all: info $(ARCH_ASM_OBJ) $(ARCH_OBJ)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile ${notdir $(ARCH_OBJ)} OK$(END)"
	@ echo -e "	$(CYAN)Compile OK!$(END)"
info:
	$(Q) echo -e "$(YELLOW)$(BOLD)[4] INTO $(UNDERLINE)$(ARCH_DIR_SINGLE)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"

#NOTE: If you use $(ARCH_ASM_FILE) instead $<, it will occur an error.
#compile assenble file
$(ARCH_ASM_OBJ): $(ARCH_ASM_FILE)
	$(Q) $(AS) $(C_FLAGS) -c $< -o $@
$(ARCH_OBJ_DIR)/%.o: $(ARCH_DIR)/$(ARCHS)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

.PHONY:info $(ARCH_ASM_OBJ)

