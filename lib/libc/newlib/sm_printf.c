/*
 * Copyright (c) 2021-2022 Amlogic, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */
#include "system.h"
#include "stdarg.h"

char myitoa_index[50] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *myitoa(int num, char *str, int radix)
{
	unsigned unum;
	int i = 0, j, k;

	if (radix == 10 && num < 0)
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;

	do
	{
		str[i++] = myitoa_index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum > 0);
	str[i] = '\0';

	if (str[0] == '-') k = 1;
	else k = 0;

	char temp;
	for (j = k; j <= (i - k - 1) / 2; j++)
	{
		temp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = temp;
	}
	return str;
}

void sm_printf(const char *fmt, ...)
{
	char ch;
	unsigned char para_flag = 0;
	int fmt_len = strlen(fmt);

	char str_data[20];
	va_list args;

	va_start(args, fmt);

	while (fmt_len --) {
		ch = *fmt++;

		// read in the format string
		if (ch == '%') {
			para_flag = 1;
			continue;
		} else if (ch == '\0') {
			putc_usart1(ch);
		} else if ((ch == '\n') || (ch == '\r')) {
			putc_usart1('\r');
		} else if (para_flag == 0) {
			putc_usart1(ch);
			continue;
		}

		if (ch == 'c' || ch == 'C')
		{
			char *cc = va_arg(args, char *);
			putc_usart1(*cc);
		} else if (ch == 's' || ch == 'S') {
			char *str = va_arg(args, char *);
			while (*str != '\0') {
				putc_usart1(*str);
				str++;
			}
		} else if (ch == 'x' || ch == 'X') {
			int pdata = va_arg(args, int);
			int i;

			myitoa(pdata, str_data, 16);
			for (i = 0; i < 20 && str_data[i] != '\0'; i++) {
				putc_usart1(str_data[i]);
			}
		} else if (ch == 'd' || ch == 'D') {
			int pdata = va_arg(args, int);
			int i;

			myitoa(pdata, str_data, 10);
			for (i = 0; i < 20 && str_data[i] != '\0'; i++) {
				putc_usart1(str_data[i]);
			}
		} else {
			putc_usart1(ch);
		}
		para_flag = 0;
	}
	va_end(args);
}
