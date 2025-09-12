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

//********************************************************
//	spfcore.c
//	Core IEEE-754 single precision floating point
//	routines for CPM-68K floating point library
//	Written by:	J. Lovrinic
//	Date:		20241028
//********************************************************

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

// zero test
spf spfzt(val)
spf val;
{
	if (val == spfNAN) return val;
	if (val == spfz) return true;
	else return false;
}

// spfcmp - compare 2 floating point numbers
long spfcmp(a, b)
spf a;
spf b;
{
	long sign_a, sign_b;
	long abs_a, abs_b;
	long work;
	if (a == b) return 0L;
	sign_a = a & 0x80000000L;
	sign_b = b & 0x80000000L;
	if (sign_a && !sign_b) return -1L;
	if (!sign_a && sign_b) return 1L;
	abs_a = a & 0x7FFFFFFFL;
	abs_b = b & 0x7FFFFFFFL;
	if (abs_a == abs_b) return 0L;
	if (!sign_a) {
		work = (abs_a < abs_b) ? -1L : 1L;
	} else {
		work = (abs_a > abs_b) ? -1L : 1L;
	}
	return work;
}

// spfdiv - floating point division
spf spfdiv(dividend, divisor)
spf dividend;
spf divisor;
{
	long divmant;	// dividend mantissa
	long dismant;	// divisor mantissa
	long divexp;	// dividend exp
	long disexp;	// divisor exp
	spf quotient;	// quotient
	long qexp;	// quotient exponent
	long j;	// loop counter
	long sign;	// sign variable
	long guard, round, sticky;
	sign = (dividend & 0x80000000L) ^ (divisor & 0x80000000L);
	// range check
	if (spfzt(divisor)) {
		quotient = spfNAN;
		printf("Bad value dividor.\n");
		return quotient;
	}
	if (spfzt(dividend)) {
		quotient = 0L;
		return quotient;
	}
	quotient = 0L;
	divmant = 0x007fffffL & dividend;
	divmant |= 0x00800000L;
	dismant = 0x007fffffL & divisor;
	dismant |= 0x00800000L;
	for (j = 1L; j <= 27L; j++) {
		quotient <<= 1L;
		if (divmant >= dismant) {
			quotient += 1L;
			divmant -= dismant;
		}
		divmant <<= 1L;
	}
	// address rounding
	guard = quotient & 0x00000004L >> 2L;
	round = (quotient & 0x00000002L) >> 1L;
	sticky = (quotient & 0x00000001L);
	if ((divmant & 0x80000000L) != 0L) {
		round = 1L;
	} else {
		round = 0L;
	}
	if (guard != 0L) {
		if ((round != 0L) || (sticky != 0L) || ((quotient & 0x00000008L) != 0L)) {
			quotient += 0x00000008L;
		}
	}
	quotient >>= 3L;
	// determine quotient exp
	divexp = (0x7f800000L & dividend) >> 23L;
	divexp -= 127L;
	disexp = (0x7f800000L & divisor) >> 23L;
	disexp -= 127L;
	qexp = divexp - disexp;
	// Handle overflow due to rounding
	if (quotient & 0x01000000L) {
		quotient >>= 1L;
		qexp += 1L;
	}
	// normalize the quotient and exponent
	while (quotient < 0x00800000L) {
		quotient <<= 1L;
		qexp -= 1L;
	}
	// convert exp format
	qexp = qexp + 127L;
	// assemble complete quotient
	// format quotient - remove leading 1
	quotient &= 0x007fffffL;
	// format and incorporate exp
	quotient |= (qexp << 23L);
	quotient |= sign;
	return quotient;
}

// floating point negation function
spf spfneg(x)
spf x;
{
	if (x == spfNAN || x == spfz) return x;
	if ((0x80000000L & x) != 0L) {
		return (x & 0x7fffffffL);
	} else {
		return (x | 0x80000000L);
	}
}

// floating point abs function
spf spfabs(x)
spf x;
{
	if (x == spfNAN) return x;
	return (0x7fffffffL & x);
}

// floating point add function
spf spfadd(x, y)
spf x;
spf y;
{
	long xmant;
	long ymant;
	long xexp;
	long yexp;
	long a, b;
	long signa;
	long signb;
	spf c;
	if (y == 0) return x;
	if (x == 0) return y;
	if (x == spfNAN) return x;
	if (y == spfNAN) return y;
	// determine dominant variable and break them down
	if (spflt(spfabs(x), spfabs(y))) {
		b = x;
		a = y;
	} else {
		a = x;
		b = y;
	}
	// extract components
	xmant = 0x007fffffL & a;
	xexp = 0x7f800000L & a;
	ymant = 0x007fffffL & b;
	yexp = 0x7f800000L & b;
	// normalize exp
	xexp >>= 23L;
	yexp >>= 23L;
	xexp -= 127L;
	yexp -= 127L;
	// normalize mantissas
	xmant |= 0x00800000L;
	ymant |= 0x00800000L;
	// adjust for simple rounding - multiply both by 2 (shift 1)
	xmant <<= 1L;
	ymant <<= 1L;
	// xexp is always the larger exp - dominant
	// check against yexp to see if y will affect the result
	c = xexp - yexp;
	if (c >= 24L) return a;
	ymant >>= c;
	signa = a & 0x80000000L;
	signb = b & 0x80000000L;
	if ((signa ^ signb) == 0L) {
		// add mantissa and normalize up
		xmant += ymant;
		if (xmant == 0L) return spfz;
		while (xmant >= 0x02000000L) {
			xmant >>= 1L;
			xexp += 1L;
		}
	} else {
		// subtract mantissa and normalize down
		xmant -= ymant;
		if (xmant == 0L) return spfz;
		while (xmant < 0x01000000L) {
			xmant <<= 1L;
			xexp -= 1L;
		}
	}
	// address rounding
	c = xmant & 0x00000003L;
	if (c == 0x00000001L) xmant += 1L;
	// assemble the result
	xmant >>= 1L;
	xmant &= 0x007fffffL;
	xexp += 127L;
	xexp <<= 23L;
	c = (a & 0x80000000L) | xexp | xmant;
	return c;
}

// floating point subtraction function
spf spfsub(a, b)
spf a;
spf b;
{
	spf work;
	if (a == spfNAN) return a;
	if (b == spfNAN) return b;
	work = spfadd(a, spfneg(b));
	if (work == 0x80000000L) work = spfz;
	return work;
}

// spfmod - floating point mod function
spf spfmod(a, b)
spf a;
spf b;
{
	spf work;
	work = spfint(spfdiv(a, b));
	work = spfmul(work, b);
	return spfsub(a, work);
}

