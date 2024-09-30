#! /bin/bash
#
# Copyright (c) 2021-2022 Simon. All rights reserved.
#
# SPDX-License-Identifier: MIT
#

END="\033[0m"
BOLD="\033[1m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
MAGENTA="\033[35m"
CYAN="\033[36m"

function print_bold(){
	case $1 in
		"RED" | "red")
			echo -e "$BOLD$RED$2$END"
			;;
		"GREEN" | "green")
			echo -e "$BOLD$GREEN$2$END"
			;;
		"YELLOW" | "yellow")
			echo -e "$BOLD$YELLOW$2$END"
			;;
		"MAGENTA" | "magenta")
			echo -e "$BOLD$MAGENTA$2$END"
			;;
		"BLUE" | "blue")
			echo -e "$BOLD$BULE$2$END"
			;;
		"CYAN" | "cyan")
			echo -e "$BOLD$CYAN$2$END"
			;;
		*)
			;;
	esac

}

function print_message(){
	case $1 in
		"RED"|"red")
			echo -e "$RED$2$END"
			;;
		"GREEN"|"green")
			echo -e "$GREEN$2$END"
			;;
		"MAGENTA"|"magenta")
			echo -e "$MAGENTA$2$END"
			;;
		"CYAN"|"cyan")
			echo -e "$CYAN$2$END"
			;;
		"YELLOW"|"yellow")
			echo -e "$YELLOW$2$END"
			;;
		*)
			;;
	esac
}

