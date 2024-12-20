/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __SM_IMPURE_H__
#define __SM_IMPURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <reent.h>

extern struct _reent *__ATTRIBUTE_IMPURE_PTR__ _impure_ptr;

void _reclaim_reent(struct _reent *reent_ptr);

#ifdef __cplusplus
}
#endif

#endif
