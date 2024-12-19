# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/output/config.h

SHELL := /bin/bash
PWD := $(shell pwd)
KERNEL_DIR := $(PWD)
#This variable is useful to obtain the directory named arch
KERNEL_DIR_SINGLE := $(shell basename $(KERNEL_DIR))

Q := @

#define compiler related parameters
C_DEFS =

#define where to find the header files, it will increased as needed in future.
C_INCLUDES = -I$(PRJ_ROOT_DIR)/lib/include \
			 -I$(KERNEL_DIR)/include \
			 -I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS)/ \
			 -I$(KERNEL_DIR)/portable/GCC/ARM_CM4F

C_FLAGS += $(MCU_PARA) $(C_DEFS) $(C_INCLUDES) -Og -Wall -fdata-sections -ffunction-sections

#define object file related variable
KERNEL_ASM_FILE := $(wildcard $(KERNEL_DIR)/*.s)
KERNEL_C_FILE := $(wildcard $(KERNEL_DIR)/*.c)
KERNEL_C_FILE_F += $(wildcard $(KERNEL_DIR)/portable/GCC/ARM_CM4F/*.c)
KERNEL_C_FILE_M += $(KERNEL_DIR)/portable/MemMang/heap_5.c

KERNEL_OBJ_DIR := $(BUILD_OUT)

KERNEL_OBJ = $(addprefix $(KERNEL_OBJ_DIR)/, $(notdir $(KERNEL_C_FILE:.c=.o)))
KERNEL_OBJ_F = $(addprefix $(KERNEL_OBJ_DIR)/, $(notdir $(KERNEL_C_FILE_F:.c=.o)))
KERNEL_OBJ_M = $(addprefix $(KERNEL_OBJ_DIR)/, $(notdir $(KERNEL_C_FILE_M:.c=.o)))

#define obj file path
KERNEL_ASM_OBJ := $(addprefix $(KERNEL_OBJ_DIR)/, $(notdir $(KERNEL_ASM_FILE:.s=.o)))
BUILD_SOURCE += $(KERNEL_ASM_OBJ)
BUILD_SOURCE += $(KERNEL_OBJ) $(KERNEL_OBJ_F) $(KERNEL_OBJ_M)

all: info $(KERNEL_ASM_OBJ) $(KERNEL_OBJ_M) $(KERNEL_OBJ_F) $(KERNEL_OBJ)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile $(KERNEL_OBJ)$(END)"
	@ echo -e "$(GREEN)Compile $(KERNEL_OBJ_F)$(END)"
	@ echo -e "$(GREEN)Compile $(KERNEL_OBJ_M)$(END)"
	@ echo -e "	$(CYAN)Compile OK!$(END)"
info:
	@ echo -e "$(YELLOW)$(BOLD)[7] INTO $(UNDERLINE)$(KERNEL_DIR_SINGLE)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"

#NOTE: If you use $(KERNEL_ASM_FILE) instead $<, it will occur an error.
#compile assenble file
$(KERNEL_ASM_OBJ): $(KERNEL_ASM_FILE)
	$(Q) $(AS) $(C_FLAGS) -c $< -o $@

$(KERNEL_OBJ_M): $(KERNEL_C_FILE_M)
	$(Q) $(CC) $(C_FLAGS) -c $(KERNEL_C_FILE_M) -o $@

$(KERNEL_OBJ_F): $(KERNEL_C_FILE_F)
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_DIR)/$(KERNELS)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

.PHONY:info $(KERNEL_ASM_OBJ)



