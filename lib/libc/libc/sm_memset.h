/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __SM_MEMSET_H__
#define __SM_MEMSET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "system.h"

void *memset(void *dest, int c, size_t len);

#ifdef __cplusplus
}
#endif

#endif
