/*
 * Copyright (c) 2021-2022 Simon, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "system.h"

unsigned int strlen(const char *s)
{
	int len = 0;

	while (*s++)
		len++;

	return len;
}
