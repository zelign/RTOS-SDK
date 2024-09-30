/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "sm_strlen.h"

int strlen(const char *s)
{
	int len = 0;

	while (*s++)
		len++;

	return len;
}
