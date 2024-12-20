/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "system.h"

int isalpha(int c)
{
	return islower(c) || isupper(c);
}
