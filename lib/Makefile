# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk

SHELL := /bin/bash
PWD := $(shell pwd)
LIB_DIR := $(PWD)
NEW_LIB_DIR := $(LIB_DIR)/libc/newlib
CLI_LIB_DIR := $(LIB_DIR)/FreeRTOS_CLI
#This variable is useful to obtain the directory named arch
LIB_DIR_SINGLE := $(shell basename $(LIB_DIR))

Q := @

#define compiler related parameters
C_DEFS =

#define where to find the header files, it will increased as needed in future.
C_INCLUDES = -I$(PRJ_ROOT_DIR)/lib/include \
			 -I$(LIB_DIR)/ \
			 -I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS)/ \
			 -I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/include \
			 -I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/portable/GCC/ARM_CM4F

C_FLAGS += $(MCU_PARA) $(C_DEFS) $(C_INCLUDES) -Og -Wall -fdata-sections -ffunction-sections

#define object file related variable
LIB_C_FILE := $(wildcard $(NEW_LIB_DIR)/*.c)
CLI_C_FILE := $(wildcard $(CLI_LIB_DIR)/*.c)

LIBC_OBJ_DIR := $(BUILD_OUT)
CLI_OBJ_DIR := $(BUILD_OUT)

LIBC_OBJ = $(addprefix $(LIBC_OBJ_DIR)/, $(notdir $(LIB_C_FILE:.c=.o)))
CLI_OBJ = $(addprefix $(CLI_OBJ_DIR)/, $(notdir $(CLI_C_FILE:.c=.o)))

#define obj file path
BUILD_SOURCE += $(LIBC_OBJ) $(CLI_OBJ)

all: info $(LIBC_OBJ) $(CLI_OBJ)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile library $(END)"
	@ echo -e "	$(CYAN)Compile OK! $(LIB_LIBC_C_FILEDIR)$(END)"
info:
	@ echo -e "$(YELLOW)$(BOLD)[8] INTO $(UNDERLINE)$(LIB_DIR_SINGLE)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"

$(LIBC_OBJ_DIR)/%.o: $(NEW_LIB_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

$(CLI_OBJ_DIR)/%.o: $(CLI_LIB_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

.PHONY:info $(KERNEL_ASM_OBJ)



