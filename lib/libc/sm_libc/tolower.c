/*
 * Copyright (c) 2021-2022 Simon, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "system.h"

int tolower(int c)
{
	return c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c;
}
