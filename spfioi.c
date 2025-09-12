// MIT License
// Copyright (c) 2025 John J Lovrinic
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//--------------------------------------------------------
// spfioi.c
// Support routines for CP/M-68K single-precision
// floating point package
// Written by: J. Lovrinic
// Date: 20241028
//--------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

#define ten		0x41200000L
#define tenth	0x3dcccccdL

// atof - string to spf
spf atof(str)
char *str;
{
	long sign;
	long esign;
	long evalue;
	long a;
	long b;
	long c;
	spf fb;
	spf fd;
	spf fi;
	spf ff;
	spf fe;
	long i;
	spf result;
	char ch;

	sign = spfz;
	esign = 1L;
	evalue = 0L;
	fi = 0L;
	ff = 0L;
	fe = 0L;

	// Skip leading whitespace
	while (isspace(*str)) str++;

	// Optional sign
	if (*str == '-') {
		sign = 0x80000000L;
		str++;
	} else if (*str == '+') {
		str++;
	}

	// Integer part
	b = 0L;
	while (isdigit(*str)) {
		ch = (char)(*str - '0');
		a = (long)ch;
		b *= 10L;
		b += a;
		str++;
	}
	fi = spfltf(b);

	// Fractional part
	i = 0L;
	if (*str == '.') {
		str++;
		b = 0L;
		while (isdigit(*str)) {
			ch = (char)(*str - '0');
			a = (long)ch;
			b *= 10L;
			b += a;
			i++;
			str++;
		}
		fd = spfpowi(spf10, i);		// scale = 10^i
		fb = spfltf(b);
		ff = spfdiv(fb, fd);
	}

	result = spfadd(fi, ff);

	// Scientific exponent (e/E)
	if (*str == 'e' || *str == 'E') {
		str++;
		if (*str == '-') {
			esign = -1L;
			str++;
		} else if (*str == '+') {
			str++;
		}
		b = 0L;
		while (isdigit(*str)) {
			ch = (char)(*str - '0');
			a = (long)ch;
			b *= 10L;
			b += a;
			str++;
		}
		fe = spfpowi(spf10, b);
		if (esign == -1L) {
			result = spfdiv(result, fe);
		} else {
			result = spfmul(result, fe);
		}
	}

	return sign | result;
}

