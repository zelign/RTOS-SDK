#! /bin/bash
#
# Copyright (c) 2021-2022 Simon. All rights reserved.
#
# SPDX-License-Identifier: MIT
#

PWD=$(pwd)
source $PWD/scripts/print.sh

BUILD_PRJ_DIR=$(realpath $(dirname ${BASH_SOURCE[0]:-$0})/..)
MAKE="Makefile"
DO_BUILD_SH=1
USE_SYSTEM_TOOLCHAIN="y"

function build_all() {
	pushd $BUILD_PRJ_DIR > /dev/null
	print_bold YELLOW "[3] START BUILD $BOARDS-$MCUS"

	if [ -n "BUILD_START" ]; then
		make --no-print-directory
	else
		echo -e "$BOLD$RED Have not find ${BUILD_PRJ_DIR}/board/$ARCHS/$MAKE file!**$END"
		exit
	fi
	if [ $? -ne 0 ]; then
		print_message red "Build rtos image fail!"
		popd > /dev/null
		exit 1
	fi
	popd > /dev/null
}

# function package_target_verify() {
# 	print_bold YELLOW "[1] START VERIFY PACKAGE TARGET"
# 	j=1
# 	while IFS= read -r LINE; do
# 		if [ $j == $CHOICE_PACKAGE ]; then
# 			PACKAGE_ARRY=$(echo "$LINE" | tr ' ' ' ')
# 			break	
# 		fi
# 		j=$[$j + 1]
# 	done <$PACKAGE_COMBINATION

# 	if [ -z "$PACKAGE_ARRY" ]; then
# 		echo -e "\033[41;33m Package list is not set, please execute source \"scripts/select_prj.sh\"\033[0m"
# 	fi
# 	j=0
# 	for temp in ${PACKAGE_ARRY[@]}; do
# 		if [ $j == 1 ]; then
# 			arch=$temp
# 		elif [ $j == 2 ]; then
# 			board=$temp
# 		elif [ $j == 3 ]; then
# 			mcu=$temp
# 		elif [ $j == 4 ]; then
# 			product=$temp
# 		fi
# 		j=$[$j + 1]
# 	done
# 	case ${product} in
# 		'demo')
# 			#read prj.cfg file to output/package/arch-board-mcu-product/build/config.h
# 			;;
# 		*)
# 			echo "Unsupported product type:${product}"
# 			exit 1
# 			;;
# 	esac
# 	BUILD_START=1
# 	ARCHS=$arch
# 	MCUS=$mcu
# 	BOARDS=$board
# 	PRODUCTS=$product
# 	echo -e "\033[34m$ARCHS $MCUS $BOARDS $PRODUCTS \033[0m"
# }


function package_env_config(){
	print_bold YELLOW "[2] START PACKAGE ENV CONFIG"
	case $1 in
		'stm32')
			UBOOT_BOARDNAME="stm32_nor_rtos"
			;;
		'stc')
			;;
		*)
			echo "Unsupported board type:$1"
			exit 1
			;;
	esac

	case $2 in
		"arm")
			COMPILER=gcc
			COMPILER_KEYWORD=arm-none-eabi
			if [ -n "$USE_SYSTEM_TOOLCHAIN" ]; then
				echo -e "\033[34muse system compile toolchain\033[0m"
			else
				TOOLCHAIN_SRC=$(realpath $BUILD_PRJ_DIR/toolchain/$ARCHS/)
				TOOLCHAIN_OUT=$BUILD_PRJ_DIR/output/toolchain/
				echo -e "\033[34mToolchain from \033[4m$TOOLCHAIN_SRC\033[0m"
				echo -e "\033[34mExtract to \033[4m$TOOLCHAIN_OUT\033[0m"
			fi
			;;
		*)
			;;
	esac

	#Arch prefix setting
	if [ -n "$ARCHS" ]; then
		ARCH_PREFIX="${ARCHS}""-"
		PRODUCT_SUFFIX="-""${PRODUCTS}"
	fi
	BUILD_CLEAN=1
}

print_bold YELLOW "[1] VERIFIED PACKAGE TARGET SUCCEED"
echo -e "\033[34m$ARCHS $MCUS $BOARDS $PRODUCTS \033[0m"
package_env_config $BOARDS $ARCHS

export COMPILER TOOLCHAIN_SRC TOOLCHAIN_OUT COMPILER_KEYWORD ARCHS BOARDS MCUS PRODUCTS DO_BUILD_SH

#store middle files
export MICRO_SDK_OUT_DIR=${BUILD_PRJ_DIR}/output/${ARCHS}-${BOARDS}-${MCUS}-${PRODUCTS}
#store all of compile files:.image .bin. .lst .map .elf
export BUILD_OUT_PACKAGE=${MICRO_SDK_OUT_DIR}/packages
#store compile middle file like *.o
export BUILD_OUT=${MICRO_SDK_OUT_DIR}/build

test -n $BUILD_CLEAN && rm -rf ${MICRO_SDK_OUT_DIR}

mkdir -p $BUILD_OUT_PACKAGE $BUILD_OUT
test -n $BUILD_START && build_all $1




