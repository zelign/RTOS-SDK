/*
 * Copyright (c) 2021-2022 Simon, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "system.h"

char *strcat(char *dst, const char *src)
{
	(void)strcpy(dst + strlen(dst), src);

	return dst;
}

