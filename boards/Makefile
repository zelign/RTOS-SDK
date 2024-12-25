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

BUILD_SOURCE += $(BOARD_C_OBJ)

all: info $(BOARD_C_OBJ)
	@ echo "BUILD_SOURCE := $(BUILD_SOURCE)" >> $(BUILD_OUT)/config.mk
	@ echo -e "$(GREEN)Compile ${notdir $(BOARD_C_OBJ)} OK$(END)"
	@ echo -e "	$(CYAN)Compile OK!$(END)"
info:
	@ echo -e "$(YELLOW)$(BOLD)[6] INTO $(UNDERLINE)$(BOARD_DIR_SIGNED)$(END) $(YELLOW)$(BOLD)COMPILE...$(END)"
$(BOARD_BUILD_DIR)/%.o: $(BOARD_DIR)/$(ARCHS)/$(BOARDS)/%.c
	@ $(CC) $(C_FLAGS) -c $< -o $@