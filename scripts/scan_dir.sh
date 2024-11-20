#! /bin/bash
#
# Copyright (c) 2021-2022 Simon. All rights reserved.
#
# SPDX-License-Identifier: MIT
#

#This script scan all of items of directories to show a list for user choice.
PWD=$(pwd) #root directory

#search all of directories from /arch to get architecture types.
echo "pwd=$PWD"
source $PWD/scripts/print.sh

ARCHS=$(find $PWD/arch -mindepth 1 -maxdepth 1 -type d ! -name ".*" | xargs basename -a | sort)
ARCHS_BOARDS=$(find $PWD/boards -mindepth 1 -maxdepth 1 -type d ! -name ".*" | xargs basename -a | sort)
PRODUCTS=$(find $PWD/products -mindepth 1 -maxdepth 1 -type d ! -name ".*" | xargs basename -a | sort)
export PACKAGE_COMBINATION="$PWD/output/package_comnination.in"

if [ ! -d  "$PWD/output" ]; then	 
	mkdir -p $PWD/output
fi

if [ ! -e "$PACKAGE_COMBINATION" ]; then
	touch "$PACKAGE_COMBINATION"
else
	: > $PACKAGE_COMBINATION
fi
echo "Available projects:"
index=0
for arch in ${ARCHS[*]}; do
	for arch_board in ${ARCHS_BOARDS[*]}; do
		if [ "$arch" = "$arch_board" ]; then
			BOARDS=$(find $PWD/boards/$arch -mindepth 1 -maxdepth 1 -type d ! -name ".*" | xargs basename -a | sort)
			for board in ${BOARDS[*]}; do
				if [ "$(find "$PWD/boards/$arch/$board/" -mindepth 1 -type d)" ]; then
					MCUS=$(find $PWD/boards/$arch/$board -mindepth 1 -maxdepth 1 -type d ! -name ".*" | xargs basename -a | sort)
					for mcu in ${MCUS[*]}; do
						for pro in ${PRODUCTS[*]}; do
							index=$[$index+1]
							echo -e "\033[1m \033[3;36m$index. \033[0m \033[3;35m$arch $board $mcu $pro\033[0m"
							ARRYS=$(echo "$index $arch $board $mcu $pro")
							echo "$ARRYS" >> $PACKAGE_COMBINATION
						done
					done
				fi
			done
		fi
	done
done

read -p "Choice your project:" CHOICE_PACKAGE

export CHOICE_PACKAGE

function package_target_verify() {
	j=1
	while IFS= read -r LINE; do
		if [ $j == $CHOICE_PACKAGE ]; then
			PACKAGE_ARRY=$(echo "$LINE" | tr ' ' ' ')
			break	
		fi
		j=$[$j + 1]
	done <$PACKAGE_COMBINATION

	if [ -z "$PACKAGE_ARRY" ]; then
		echo -e "\033[41;33m Package list is not set, please execute source \"scripts/select_prj.sh\"\033[0m"
	fi
	j=0
	for temp in ${PACKAGE_ARRY[@]}; do
		if [ $j == 1 ]; then
			arch=$temp
		elif [ $j == 2 ]; then
			board=$temp
		elif [ $j == 3 ]; then
			mcu=$temp
		elif [ $j == 4 ]; then
			product=$temp
		fi
		j=$[$j + 1]
	done
	case ${product} in
		'demo')
			#read prj.cfg file to output/package/arch-board-mcu-product/build/config.h
			;;
		*)
			echo "Unsupported product type:${product}"
			exit 1
			;;
	esac
	BUILD_START=1
	ARCHS=$arch
	MCUS=$mcu
	BOARDS=$board
	PRODUCTS=$product
}

package_target_verify
export ARCHS BOARDS MCUS PRODUCTS
