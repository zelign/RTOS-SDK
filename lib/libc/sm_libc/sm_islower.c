/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "sm_islower.h"

int islower(int c)
{
	return c >= 'a' && c <= 'z';
}
