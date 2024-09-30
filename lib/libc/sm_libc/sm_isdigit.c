/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "sm_isdigit.h"

int isdigit(int c)
{
	return c >= '0' && c <= '9';
}
