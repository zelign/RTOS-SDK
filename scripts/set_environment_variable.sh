#! /bin/bash
#
# Copyright (c) 2021-2022 Simon. All rights reserved.
#
# SPDX-License-Identifier: MIT
#

PWD=$(pwd)
source $PWD/scripts/print.sh

print_bold BLUE "Start enviroment configuration..."

usage()
{
	echo "$(shell tput smso)Usage: source $BASH_SOURCE $ARCH $BOARD $MCU"
	echo "-h:display help."
	echo ""
}

if [ -n "$1" ] && [ $1 == "-h" ]; then
	usage
	return 0
elif [ $# -eq 4 ]; then
	PROJECT="$1 $2 $3 $4"
	print_bold GREEN "Choose project:$PROJECT"
elif [ $# -eq 5 ]; then
	PROJECT="$1 $2 $3 $4 $5"
	print_bold GREEN "Choose project:$PROJECT"
else
	print_message red "Unsupport project!"
fi


