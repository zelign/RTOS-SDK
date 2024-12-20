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

#define MEMSET_SUCCESS 0
#define MEMSET_COUNT_ERROR 1
#define MEMSET_DST_ERROR 2

.weak memset
.type memset, %function
// int *memset(void *dst, int c, size_t count)
memset:
	cmp r2, #0
	beq count_error // if count == 0, return

	cmp r0, #0
	beq dst_error // if dst == NULL, return

	cmp r2, #4
	blt 0f // if count < 4, fill byte by byte
0:
	strb r1, [r0] // fill the data into dst
	add r0, r0, #1 // increment dst
	sub r2, r2, #1	// decrement count
	cmp r2, #0	// check if count is 0
	beq end	// if count is 0, end the loop
	b 0b	// if count is not 0, loop again
1:
	and r3, r0, #3	// check if dst is word aligned
	bge word_fill // if dst is word aligned, fill word by word
	blt byte_fill // if dst is not word aligned, fill byte by byte

byte_fill:
	strb r1, [r0]// fill the byte data into dst
	add r0, r0, #1 // increment dst
	sub r2, r2, #1	// decrement count
	cmp r2, #0	// check if count is 0
	beq end	// if count is 0, end the loop
	b 1b	// if count is not 0, loop again

word_fill:
	cmp r2, #4	// check if count is less than 4
	blt byte_fill // if count is less than 4, fill byte by byte
	strb r1, [r0]	// fill the word data into dst
	add r0, r0, #4 // increment dst
	sub r2, r2, #4	// decrement count
	cmp r2, #0	// check if count is 0
	beq end	// if count is 0, end the loop
	b 1b	// if count is not 0, loop again

count_error:
	mov r0, #MEMSET_COUNT_ERROR
	bx lr
dst_error:
	mov r0, #MEMSET_DST_ERROR
	bx lr
end:
	mov r0, #MEMSET_SUCCESS
	bx lr

.end