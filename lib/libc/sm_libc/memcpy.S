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

.weak memcpy
.type memcpy, %function

#define MEMCPY_SUCCESS 0
#define MEMCPY_COUNT_ERROR 1
#define MEMCPY_DST_ERROR 2
#define MEMCPY_SRC_ERROR 3

// int *memcpy(void *dst, const void *src, unsigned int count)
//r0: dst, r1: src, r2: count
memcpy:
	cmp r2, #0	// check if count is 0
	beq count_fail // if count == 0, return

	cmp r0, #0 // check if dst is 0
	beq dst_fail // if dst == 0, return

	cmp r1, #0	// check if src is 0
	beq src_fail // if src == 0, return

	cmp r2, #4	// check if count is less than 4
	beq	byte_copy // if count is less than 4, copy byte by byte

	cmp r0, r1 // compare the size between dst and src to get the offset to check alignment
	blt 1f // if dst is less than src
	b 2f
1: 	// if dst is less than src
	sub r3, r1, r0
	b 3f
2: 	// if dst is greater than src
	sub r3, r0, r1
	b 3f
3:
	and r3, #3 //if r3 & 3 == 0, dst and src will be word aligned, otherwise, directly copy byte by byte
	beq check_alignment
	b byte_copy

byte_copy:
	ldrb r3, [r1]  // load the byte data from src to r3
	add r1 , r1, #1 // increment src
	strb r3, [r0] // store the byte data from r3 to dst
	add r0, r0, #1 // increment dst
	sub r2, r2, #1 // decrement count
	cmp r2, #0 // check if count is 0
	beq end // if count is 0, end the loop
	b byte_copy // if count is not 0, loop again

check_alignment:
	and r0, #3 //only check dst alignment, src have equal alignment with dst
	beq word_copy // the dst and src is word aligned, copy word by word

	// the begin of dst and src is not word aligned, copy byte by byte
	ldrb r3, [r1] // read src data to r3 register
	add r1, r1, #1 // src address increment
	strb r3, [r0] // write the data stored in r3 to dst
	add r0, r0, #1 // dst address increment
	sub r2, r2, #1 // count decrement
	b check_alignment // again check alignment

word_copy:
	cmp r2, #4 // if the remaining count is less than 4, copy byte by byte
	blt byte_copy

	ldr r3, [r1]	// load the word data from src to r3
	add r1, r1, #4	// increment src
	str r3, [r0]	// store the word data from r3 to dst
	add r0, r0, #4	// increment dst
	sub r2, r2, #4	// decrement count
	cmp r2, #0		// check if count is 0
	beq end			// if count is 0, end the function
	b word_copy		// if count is not 0, loop again to copy word by word

count_fail:
	mov r0, #MEMCPY_COUNT_ERROR
	b return
dst_fail:
	mov r0, #MEMCPY_DST_ERROR
	b return
src_fail:
	mov r0, #MEMCPY_SRC_ERROR
	b return
end:
	mov r0, #MEMCPY_SUCCESS
return:
	bx lr
.end