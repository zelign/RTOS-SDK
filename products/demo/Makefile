# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

include $(BUILD_OUT)/config.mk
include $(PRJ_ROOT_DIR)/scripts/fonts_color.mk

SHELL := /bin/bash
PWD := $(shell pwd)

DEMO_DIR := $(PWD)
DEMO_DIR_SINGLE := $(shell basename $(DEMO_DIR))

INCLUDE_DIR := $(DEMO_DIR)/
DEMO_SRC_FILE := $(wildcard $(DEMO_DIR)/*.c)

BUILD_SOURCE += $(BUILD_OUT)/main.o

C_DEFS :=
C_INCLUDES := -I$(INCLUDE_DIR) \
			  -I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS) \
			  -I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/include \
			  -I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS)/$(MCUS) \
			  -I$(PRJ_ROOT_DIR)/lib/include \
			  -I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/cortex-M4 \
			  -I$(PRJ_ROOT_DIR)/kernel/FreeRTOS \
			  -I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/include \
			  -I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/portable/GCC/ARM_CM4F \
			  -I$(DRV_INCLUDES) \
			  -I$(PRJ_ROOT_DIR)/output

C_FLAGS += $(MCU_PARA) $(C_DEFS) $(C_INCLUDES) -Og -Wall -fdata-sections -ffunction-sections
DEMO_OBJ_DIR := $(BUILD_OUT)
DEMO_C_OBJ := $(addprefix $(DEMO_OBJ_DIR)/, $(notdir $(DEMO_SRC_FILE:.c=.o)))

all: info build_obj

build_obj: $(DEMO_C_OBJ)  build_prj
	@ echo -e "$(GREEN)$(BOLD)****************** COMPLETED BUILD ALL ELF_FILE! ******************$(END)"

build_prj: $(HEX_FILE)
	@ $(DP) --source --all-headers --demangle --file-headers --line-numbers --wide $(ELF_FILE) > $(LST_FILE)

#BUILD_SOURCE, it define is top of this file.
$(ELF_FILE): $(BUILD_SOURCE)
	@ echo -e "$(GREEN)$(BOLD)*********************** START LINK ALL OBJ! ***********************$(END)"
	@ $(CC) $(BUILD_SOURCE) $(LD_OPTION) -o $@

$(HEX_FILE): $(ELF_FILE)
	@ $(CP) -O ihex $< $@


$(DEMO_C_OBJ): $(DEMO_SRC_FILE)
	@ $(CC) $(C_FLAGS) -c $< -o $@
	@ echo -e "$(GREEN)Compile ${notdir $(DEMO_C_OBJ)} OK$(END)"
	@ echo -e "	$(CYAN)Compile OK!$(END)"

info:
	@ echo -e "$(YELLOW)$(BOLD)[9] INTO $(UNDERLINE)$(DEMO_DIR_SINGLE)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"
.PHONY: $(DEMO_C_OBJ) info
