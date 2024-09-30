/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#include "sm_isalpha.h"
#include "sm_isupper.h"
#include "sm_islower.h"

int isalpha(int c)
{
	return islower(c) || isupper(c);
}
