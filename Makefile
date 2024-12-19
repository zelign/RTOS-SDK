# Copyright (c) 2021-2022 Simon. All rights reserved.

# SPDX-License-Identifier: MIT

#NOTE: The Makefile will create a directory for toolchain, and the executation procedure
#is top makefile(build)->build.sh->top makefile(build_obj_first)->arch makefile->board make file->...->
#product makefile->top makefile(link all object files)
#====================================================================#
#These variables are need to export to environment virable by build.sh:
#TOOLCHAIN_OUT: toolchain extract path.
#COMPILER: compile name, like as `gcc`
#COMPILER_KEYWORD: like as `arm-none-eabi`
#TOOLCHAIN_SRC: the directory that store toolchain that have not be unziped
#BOARDS
#MCUS
#PRODUCTS
#ARCHS
#BUILD_OUT_PACKAGE: output/package
#BUILD_OUT: the variable will is used in late Makefile, it's value is output/xx-xx-xx-xx/build.
#BUILD_SOURCE: the variable is exported by Makefile in product/demo, it contain
#all object files(.o)
#====================================================================#

#Specify the location where store shell tool.
SHELL := /bin/bash

#get current path
CURDIR := $(PWD)

-include $(CURDIR)/scripts/fonts_color.mk

#define message format
BASH_COMPILE = echo "$(TERM_BOLD)Compiler:$(1)$(TERM_RESERT)"
MESSAGE = echo "$(TERM_BOLD)>>> $(1)$(TERM_RESERT) "
TERM_BOLD := $(shell tput smso 2>/dev/null)
TERM_RESERT := $(shell tput rmso 2>/dev/null)
Q = @
WRAP = 1
WRAP_FLAGS :=

ifeq ($(WRAP), 1)
	WRAP_FLAGS += -Wl,--wrap=_malloc_r -Wl,--wrap=_free_r
endif	

#define directory of top level
OUTPUT_DIR := $(CURDIR)/output
PRJ_ROOT_DIR := $(CURDIR)

#define compiler related paremeters
COMPILER_DIR := $(PRJ_ROOT_DIR)/output/toolchain/bin
# COMPILER_PREFIX = $(COMPILER_DIR)/arm-none-eabi-
COMPILER_PREFIX = arm-none-eabi-
CC = $(COMPILER_PREFIX)gcc
AS = $(COMPILER_PREFIX)gcc -x assembler-with-cpp
CP = $(COMPILER_PREFIX)objcopy
SZ = $(COMPILER_PREFIX)size
DP = $(COMPILER_PREFIX)objdump
LD = $(COMPILER_PREFIX)ld

CPU_PARA = -mcpu=cortex-m4
FPU_PARA = -mfpu=fpv4-sp-d16
FLOAT_ABI = -mfloat-abi=hard
#if use FLOAT_ABI, there will occur error during compiling.
MCU_PARA = $(CPU_PARA) -mthumb $(FPU_PARA) $(FLOAT_ABI)
LIB_DIR = -v
LIBS = #-nostdlib#-specs=nano.specs #

#These parameters can be passed by the script file "build.sh", so
#they are have no value before implement the command `make build`.
#So, I will take a policy to void the invalide exectation of top Makefile.
ifeq ($(DO_BUILD_SH),1)
LD_SCRIPT = $(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS)/lscript.ld
ELF_FILE = $(BUILD_OUT_PACKAGE)/$(BOARDS)-$(MCUS).elf
HEX_FILE = $(BUILD_OUT_PACKAGE)/$(BOARDS)-$(MCUS).hex
LST_FILE = $(BUILD_OUT_PACKAGE)/$(BOARDS)-$(MCUS).lst

LD_OPTION = $(MCU_PARA) -T$(LD_SCRIPT) $(LIBS) $(LIB_DIR) -Wl,-Map=$(BUILD_OUT_PACKAGE)/$(BOARDS)-$(MCUS).map,--cref,--gc-sections $(WRAP_FLAGS)#, -ffunction-sections
export CC AS CP SZ DP MCU_PARA ELF_FILE HEX_FILE LST_FILE LD_OPTION PRJ_ROOT_DIR
CROSS_COMPILER := $(TOOLCHAIN_SRC)/$(COMPILER)*$(COMPILER_KEYWORD)

#####################################################################
#These function block is to obtian which directories can be added in
#the directory list to compiler to find the header files.
#The prj.cfg file define which directory will be addedin the directory
#list of find path.
#####################################################################
#define a function block to traverse every line of configuration file
#the function read every line from configuration file, then check if
#the CONFIG_XXX equal y, if it is, then extract the first string and assign
#it to DRV_CFG.

CFG_FILE := $(PRJ_ROOT_DIR)/products/$(PRODUCTS)/prj.cfg
CFG_FILE_LINE := $(shell cat $(CFG_FILE) 2> /dev/null)

#define drivers's macro configuration
DRV_CFG :=
DRV_INCLUDES :=
DRV_C_FILE :=
m :=

define process_line
	ifeq ($(findstring =y, $(1)), =y)
		DRV_CFG += $(firstword $(subst =, ,$(1)))
	endif
endef

#This define will translate upcase to lowcase 
define up_to_low
	LOW_CFG += $(shell echo "$(1)" | tr 'A-Z' 'a-z')
endef

#This define will convert the configuration parameters into a include option
define get_find_dir
	ifeq ($(wildcard $(PRJ_ROOT_DIR)/drivers/$(patsubst config_%,%,$(1))),)
		DRV_FIND_DIR +=
		DRV_INCLUDES +=
	else
		DRV_FIND_DIR += $(PRJ_ROOT_DIR)/drivers/$(patsubst config_%,%,$(1))
		DRV_INCLUDES += -I$(PRJ_ROOT_DIR)/drivers/$(patsubst config_%,%,$(1))
	endif	
endef

define config_h_start
	@if [ ! -f $(PRJ_ROOT_DIR)/output/config.h ]; then \
		printf "#ifndef __CONFIG_H__\n" >> $(PRJ_ROOT_DIR)/output/config.h; \
		printf "#define __CONFIG_H__\n" >> $(PRJ_ROOT_DIR)/output/config.h; \
		printf "\n" >> $(PRJ_ROOT_DIR)/output/config.h; \
	else \
		rm -f $(PRJ_ROOT_DIR)/output/config.h; \
		printf "#ifndef __CONFIG_H__\n" >> $(PRJ_ROOT_DIR)/output/config.h; \
		printf "#define __CONFIG_H__\n" >> $(PRJ_ROOT_DIR)/output/config.h; \
		printf "\n" >> $(PRJ_ROOT_DIR)/output/config.h; \
	fi
endef

define config_h_end
	@ printf "#endif\n" >>$(PRJ_ROOT_DIR)/output/config.h
endef

define config_h
	@ echo -e "$(CYAN)$(BOLD)Genarete$(END) $(GREEN)config.h$(END) $(CYAN)$(BOLD)from$(END) $(GREEN)$(CFG_FILE)$(END)"
	@ printf "#define$(subst  ,,$(1))\n" >>$(PRJ_ROOT_DIR)/output/config.h
endef

$(foreach line,$(CFG_FILE_LINE),$(eval $(call process_line,$(line))))
$(foreach line,$(DRV_CFG),$(eval $(call up_to_low,$(line))))
$(foreach line,$(LOW_CFG),$(eval $(call get_find_dir, $(line))))

DRV_C_SRC_INCLUDES += $(DRV_INCLUDES) \
					-I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS) \
					-I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS)/$(MCUS) \
					-I$(PRJ_ROOT_DIR)/lib/include \
					-I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/include \
					-I$(PRJ_ROOT_DIR)/arch/$(ARCHS)/cortex-M4 \
					-I$(PRJ_ROOT_DIR)/lib/FreeRTOS_CLI \
					-I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/include \
					-I$(PRJ_ROOT_DIR)/boards/$(ARCHS)/$(BOARDS) \
					-I$(PRJ_ROOT_DIR)/kernel/FreeRTOS/portable/GCC/ARM_CM4F

DRV_FLAGS += $(MCU_PARA) $(C_DEFS) $(DRV_C_SRC_INCLUDES) -Og -Wall -fdata-sections -ffunction-sections
C_FLAGS := $(DRV_FLAGS) #$(WRAP_FLAGS)
export C_FLAGS DRV_FIND_DIR DRV_FLAGS DRV_INCLUDES

#sub Makefile, and the execution sequence is determined by the variable.
BUILD_OBJ_DIR := $(CURDIR)/arch $(CURDIR)/drivers $(DRV_FIND_DIR) $(CURDIR)/boards $(CURDIR)/kernel/FreeRTOS $(CURDIR)/lib $(CURDIR)/products/$(PRODUCTS)
endif #DO_BUILD_SH
#----------------------parameters init over ---------------------------------#

#If directly execute `make` command, the Makefile will perform form all lable: all
#That is to say, the lable all is the entry of this Makefile.
#all: toolchain build_obj_first
all: build_obj_first
build:
	$(Q) $(call BASH_COMPILE, "arm-none-eabi-gcc")
	$(Q) ./scripts/build.sh

#this lable will extract toolchain to specific directory.
toolchain:
	$(Q) if [ ! -d "$(TOOLCHAIN_OUT)" ]; then \
		$(call MESSAGE, "Extracting debug toolchain "); \
		mkdir -p $(TOOLCHAIN_OUT); \
		tar -xf $(CROSS_COMPILER).tar.xz -C $(TOOLCHAIN_OUT) --strip-components=1; \
		ls -al $(TOOLCHAIN_OUT); \
		$(TOOLCHAIN_OUT)bin/$(COMPILER_KEYWORD)-$(COMPILER) -v; \
	else \
		echo -e "$(BLUE)The toolchain has be in $(UNDERLINE)$(TOOLCHAIN_OUT)$(END)"; \
	fi

$(BUILD_OBJ_DIR):
	$(Q) $(MAKE) all -C $@

config.mk:
	$(Q) echo "BUILD_SOURCE := " >> $(BUILD_OUT)/config.mk

config.h:
	$(call config_h_start)
	$(foreach line,$(DRV_CFG),$(call config_h, $(line)))
	$(call config_h_end)

build_obj_first: config.mk config.h $(BUILD_OBJ_DIR)

clean:
	$(Q) rm -v $(OUTPUT_DIR)/arm*/build/*.o $(OUTPUT_DIR)/arm*/packages/*

dist:
	$(Q) rm -rfv $(OUTPUT_DIR)

export KCONFIG_CONFIG := ./output/${ARCHS}-${BOARDS}-${MCUS}-${PRODUCTS}/packages/.config
menuconfig:
	python3 ./scripts/menuconfig.py

.PHONY: $(BUILD_OBJ_DIR) toolchain build clean

	

