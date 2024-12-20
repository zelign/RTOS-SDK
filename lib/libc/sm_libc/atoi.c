/*
 * Copyright (c) 2021-2022 Simon, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "atoi.h"
#include "isspace.h"
#include "isdigit.h"

int atoi(const char *nptr)
{
	int result = 0;
	int neg = 0;
	char c = '\0';

	while ((c = *nptr++) && isspace(c))
		;

	if (c == '-') {
		neg = 1;
		c = *nptr++;
	}

	while (isdigit(c)) {
		result = result * 10 + (c - '0');
		c = *nptr++;
	}

	return neg ? -result : result;
}
