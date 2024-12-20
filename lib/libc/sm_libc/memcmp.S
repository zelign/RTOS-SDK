/*
 * Copyright (c) 2021-2022 Simon, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */
//#include "system.h"
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

#define MEMCMP_EQUAL 0
#define MEMCMP_NOT_EQUAL 1
#define MEMCMP_FAIL 2

.weak memcmp
.type memcmp, %function
// int *memcmp(const void *dst, const void *src, unsigned int n)
memcmp:
	cmp r2, #0
	beq fail // if count == 0, return

	cmp r0, #0
	beq fail // if dst == 0, return

	cmp r1, #0
	beq fail // if src == 0, return

	cmp r2, #4
	beq byte_compare // if count is less than 4, compare byte by byte

byte_compare:
	ldrb r3, [r0]   // load byte data from dst to r3
	ldrb r4, [r1]	// load byte data from src to r4
	cmp r3, r4 		// compare byte data
	bne no_equal	// if not equal, return
	add r0, r0, #1	// increment dst
	add r1, r1, #1 		// increment src
	sub r2, r2, #1	// decrement count
	cmp r2, #0	// check if count is 0
	beq equal	// if count is 0, return
	b byte_compare

fail:
	mov r0, #MEMCMP_FAIL
	b return
no_equal:
	mov r0, #MEMCMP_NOT_EQUAL
	b return
equal:
	mov r0, #MEMCMP_EQUAL
	b return
return:
	bx lr
.end