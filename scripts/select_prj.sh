#! /bin/bash
#
# Copyright (c) 2021-2022 Simon. All rights reserved.
#
# SPDX-License-Identifier: MIT
#

#get gcc version number and install position, what need to be
#noted is that the gcc do not work in the product, this gcc
#is unuseful for this product.

GCC_FILE=$(which gcc)
if [ -z "$GCC_FILE" ]; then
	echo "gcc is not exist!"
else
	echo "The gcc locate: ${GCC_FILE}"
fi
source scripts/scan_dir.sh

