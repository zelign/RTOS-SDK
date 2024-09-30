/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __SM_SNPRINTF_H__
#define __SM_SNPRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

int snprintf(char *str, size_t size, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
