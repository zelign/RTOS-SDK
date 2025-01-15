# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk
include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/output/.config

SHELL := /bin/bash
PWD := $(shell pwd)
LIB_DIR := $(PWD)
SM_LIB_DIR := $(LIB_DIR)/libc/sm_libc
CLI_LIB_DIR := $(LIB_DIR)/FreeRTOS_CLI
FS_LIB_DIR =

#This variable is useful to obtain the directory named arch
LIB_DIR_SINGLE := $(shell basename $(LIB_DIR))

Q := @

#define object file related variable
SM_LIB_C_FILE := $(wildcard $(SM_LIB_DIR)/*.c)
SM_LIB_S_FILE := $(wildcard $(SM_LIB_DIR)/*.S)

CLI_C_FILE := $(wildcard $(CLI_LIB_DIR)/*.c)

LIBC_OBJ_DIR := $(BUILD_OUT)
CLI_OBJ_DIR := $(BUILD_OUT)

SM_LIB_C_OBJ = $(addprefix $(LIBC_OBJ_DIR)/, $(notdir $(SM_LIB_C_FILE:.c=.o)))
SM_LIB_S_OBJ = $(addprefix $(LIBC_OBJ_DIR)/, $(notdir $(SM_LIB_S_FILE:.S=.o)))

CLI_OBJ = $(addprefix $(CLI_OBJ_DIR)/, $(notdir $(CLI_C_FILE:.c=.o)))

#define obj file path
BUILD_SOURCE += $(SM_LIB_C_OBJ) $(CLI_OBJ) $(SM_LIB_S_OBJ)

ifeq ($(CONFIG_FS), y)
FS_LIB_DIR += $(LIB_DIR)/file_system
ifeq ($(CONFIG_FILEX), y)
FX_C_FILE = $(wildcard $(LIB_DIR)/file_system/FileX/samples/*.c)
FX_C_FILE += $(wildcard $(LIB_DIR)/file_system/FileX/common/src/*.c)
FX_OBJ = $(addprefix $(BUILD_OUT)/, $(notdir $(FX_C_FILE:.c=.o)))
BUILD_SOURCE += $(FX_OBJ)
endif
endif


all: info $(SM_LIB_C_OBJ) $(SM_LIB_S_OBJ) $(CLI_OBJ) $(FS_LIB_DIR)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile library:${notdir ${SM_LIB_C_OBJ}} ${notdir ${SM_LIB_S_OBJ}} ${notdir ${CLI_OBJ}} OK$(END)"
	@ echo -e "	$(CYAN)Compile OK! $(LIB_LIBC_C_FILEDIR)$(END)"
info:
	@ echo -e "$(YELLOW)$(BOLD)[8] INTO $(UNDERLINE)$(LIB_DIR_SINGLE)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"

$(LIBC_OBJ_DIR)/%.o: $(SM_LIB_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

$(LIBC_OBJ_DIR)/%.o: $(SM_LIB_DIR)/%.S
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

$(CLI_OBJ_DIR)/%.o: $(CLI_LIB_DIR)/%.c
	$(Q) $(CC) $(C_FLAGS) -c $^ -o $@

$(FS_LIB_DIR):
	$(Q) $(MAKE) all -C $@

.PHONY:info all $(FS_LIB_DIR)



