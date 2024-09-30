/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */
#include "sm_printf.h"
#include "sm_vsnprintf.h"
#include "system.h"
#include "sm_strlen.h"

#define MAX_BUFFER_LEN 512

static char printbuffer[MAX_BUFFER_LEN];

int printf(const char *fmt, ...)
{
	va_list args;
	char *p = printbuffer;
	int n = 0;

	va_start(args, fmt);
	vsnprintf(p, MAX_BUFFER_LEN, fmt, args);
	va_end(args);

	while (*p) {
		if ('\n' == *p) {
			putc_usart1((char)'\r');
			n++;
		}
		putc_usart1((char)(*p));
		n++;
		p++;
	}

	return n;
}

int iprintf(const char *fmt, ...)
{
	va_list args;
	int n = 0;

	va_start(args, fmt);
	n = printf(fmt, args);
	va_end(args);

	return n;
}

int puts(const char *str)
{
	puts_usart1(str);
	return strlen(str);
}
