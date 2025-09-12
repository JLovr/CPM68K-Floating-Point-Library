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
//	spfefs.c
//	Single precision floating point library
//	Support routines for elementary functions
//	Includes conversions, square root, and several power functions
//	Date:		2024-10-24
//	Written by:	J. Lovrinic
//********************************************************

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

//--------------------------------------------------------
// support functions
//--------------------------------------------------------

// spfdtr - input is degrees, returns radians
spf spfdtr(deg)
spf deg;
{
	spf cpio180 = 0x3c8efa35L;	// constant pi/180
	spf res;
	res = spfmul(cpio180, deg);
	return res;
}

// spfrtd - input is radians, output is degrees
spf spfrtd(rad)
spf rad;
{
	spf k;
	spf c180opi = 0x42652EE1L;	// constant 180/pi
	k = spfmul(c180opi, rad);
	return k;
}

//********************************************************************
// spfsr4 - square root (bitwise) for 68K
// Bitwise square root over a q4.28-style mantissa. Handles integer
// and fractional phases with a shift strategy that protects values
// below 1. Produces a 24-bit mantissa suitable for recombining with
// exponent/sign in IEEE-754 single.
//********************************************************************
q4f spfsr4(x)
q4f x;
{
	unsigned long y, temp, bit;
	long i, flag, flag1;
	y = 0L;
	bit = 0x40000000L;
	// flag: integer vs fractional phase
	flag = 0L;
	// flag1: lock into fractional methodology once entered
	flag1 = 0L;
	// loop to calculate square root
	for (i = 0L; i < 25L; i++) {
		temp = y + bit;
		if (((y >> 1L) >= 0x10000000L) || flag == 0L) {
			// process integer component
			if (x >= temp) {
				x -= temp;
				y = (y >> 1L) + bit;
				flag = 1L;
			} else {
				y >>= 1L;
			}
			bit >>= 2L;
		} else {
			// process fractional component
			if (x >= temp) {
				x -= temp;
				y = y + (bit << 1L);
			}
			bit >>= 1L;
			x <<= 1L;
			flag1 = 1L;
		}
		if (x == 0L) break;
	}
	if (flag1 == 0) {
		y >>= 1L;
	}
	// rounding
	if ((y & 0x00000033L) == 0x00000001L) {
		y += 1L;
	}
	return y;
}

// spfint - integer component of a float (rounds toward -inf for frac removal)
spf spfint(num)
spf num;
{
	long sign;
	long exp, exph;
	long mant;
	spf result;
	long shift;
	if (num == spfz) return 0L;
	if (num == spfnz) return 0L;
	sign = num & 0x80000000L;
	exp = num & 0x7f800000L;
	exph = exp;
	exp >>= 23L;
	exp -= 127L;
	mant = num & 0x007fffffL;
	if (exp < 0L) return 0L;
	if (exp >= 31L) return num;
	shift = exp - 23L;
	if (shift >= 0L) {
		mant <<= shift;
		mant >>= shift;
	} else {
		mant >>= (-shift);
		mant <<= (-shift);
	}
	result = sign | exph | mant;
	return result;
}

// spfrnd - IEEE-754 round-to-nearest-even
spf spfrnd(x)
spf x;
{
	long fixbits;
	spf fix, frac;
	if (spfeq(x, spfz) || spfeq(x, spfnz)) return x;
	fix = spfint(x);
	frac = spfsub(x, fix);
	if (spfgt(frac, spfhlf)) {
		return (spfgt(x, spfz)) ? spfadd(fix, spfltf(1L)) : spfsub(fix, spfltf(1L));
	} else if (spfeq(frac, spfhlf)) {
		fixbits = spflng(fix);
		if (fixbits & 0x1) {
			return (spfgt(x, spfz)) ? spfadd(fix, spfltf(1L)) : spfsub(fix, spfltf(1L));
		} else {
			return fix;
		}
	}
	return fix;
}

// spfflr - floor(x): largest integer ≤ x
spf spfflr(x)
spf x;
{
	spf fix, frac;
	if (spfeq(x, spfz) || spfeq(x, spfnz)) return x;
	// NaN/Inf passthrough (library-specific sentinels)
	if (spfge(x, spfNAN) || spfle(x, -spfNAN)) return x;
	fix = spfint(x);
	frac = spfsub(x, fix);
	if (spflt(x, spfz) && spfne(frac, spfz)) return spfsub(fix, spfltf(1L));
	return fix;
}

// spfcl - ceil(x)
spf spfcl(x)
spf x;
{
	spf work;
	work = spfrnd(spfadd(x, spfhlf));
	return work;
}

// spfgfr - fractional part of x
spf spfgfr(x)
spf x;
{
	spf frac;
	spf work;
	work = spfabs(x);
	frac = spfsub(work, spfint(work));
	if (x < spfz) frac = spfsub(spfz, frac);
	return frac;
}

// spfltf - convert long -> float (spf)
spf spfltf(num)
long num;
{
	long mant;
	long sign;
	long exp;
	if (num == 0L) return spfz;
	mant = num;
	if (num < 0L) {
		sign = 0x80000000L;
		mant = 0 - mant;
	} else {
		sign = 0L;
	}
	exp = 31L;
	while (exp > 0L) {
		if ((mant & 0x80000000L) == 0x80000000L) break;
		exp -= 1L;
		mant <<= 1L;
	}
	mant >>= 8L;
	mant &= 0x007fffffL;
	exp = (exp + 127L);
	exp <<= 23L;
	mant |= exp;
	mant |= sign;
	return mant;
}

// spflng - convert float (spf) -> long
long spflng(val)
spf val;
{
	long exp;
	long sign;
	long mant;
	spf maxint;
	spf minint;
	maxint = 0x4F7FFFFFL;
	minint = 0xCF000000L;
	if (val == 0x00000000L) return spfz;
	if (spfgt(val, maxint)) return MAXINT;
	if (spflt(val, minint)) return MININT;
	sign = val & 0x80000000L;
	exp = val & 0x7F800000L;
	exp >>= 23L;
	exp -= 127L;
	mant = val & 0x007FFFFFL;
	mant |= 0x00800000L;
	exp -= 23L;
	if (exp >= 0L) {
		mant <<= exp;
	} else {
		mant >>= abs(exp);
	}
	if (sign) {
		mant = 0L - mant;
	}
	return mant;
}

//--------------------------------------------------------
// Q-format conversions
// q2.30 accepts -2 < x < 2, suitable for sin/cos kernels
//--------------------------------------------------------

// IEEE-754 single -> q2.30
q2f fp_q2(x)
spf x;
{
	unsigned long ux;
	long efield, exp;
	unsigned long mant;
	long sign;
	q2f result;
	long ieee2 = 0x40000000L;	// +2.0f
	long ieeen2 = 0xC0000000L;	// -2.0f
	if (spflt(x, ieeen2)) return 0x80000000L;
	if (spfgt(x, ieee2)) return 0x7FFFFFFFL;
	ux = (unsigned long)x;
	sign = (long)(ux & (unsigned long)0x80000000L);
	efield = (long)((ux >> 23) & (unsigned long)0x000000FFL);
	if (efield == 0L) {
		mant = (unsigned long)(ux & (unsigned long)0x007FFFFFL);
		if (mant == (unsigned long)0L) return 0L;
		exp = -126L;
	} else {
		mant = (unsigned long)((ux & (unsigned long)0x007FFFFFL) | (unsigned long)0x00800000L);
		exp = efield - 127L;
	}
	{
		long shift = exp + 7L;
		if (shift >= 31L) {
			result = 0x7FFFFFFFL;
		} else if (shift >= 0L) {
			result = (q2f)(mant << shift);
		} else {
			long r = -shift;
			if (r > 30L) return 0L;
			if (r > 0L) {
				unsigned long half = (unsigned long)1L << (r - 1);
				unsigned long lsb = ((mant >> r) & (unsigned long)1L);
				unsigned long mask = ((unsigned long)1L << r) - (unsigned long)1L;
				unsigned long frac = mant & mask;
				unsigned long add = ((frac > half) || (frac == half && lsb != (unsigned long)0L)) ? half : (unsigned long)0L;
				mant += add;
			}
			result = (q2f)(mant >> r);
		}
	}
	if (sign) result = -result;
	return result;
}

// q2.30 -> IEEE-754 single
spf q2_fp(x)
q2f x;
{
	long sign;
	long work;
	long exp;
	if (x == 0l) return x;
	sign = x & 0x80000000L;
	work = x;
	if (sign != 0L) {
		work = 0L - work;
	}
	exp = 1L;
	while ((work & 0x80000000L) != 0x80000000L) {
		exp--;
		work <<= 1L;
	}
	exp += 127L;
	work &= 0x7fffffffL;
	work >>= 8L;
	exp <<= 23L;
	return (work | exp | sign);
}

// q4.28 -> IEEE-754 single
spf q4_fp(x)
q4f x;
{
	long sign;
	long work;
	long exp;
	if (x == 0l) return x;
	sign = x & 0x80000000L;
	work = x;
	if (sign != 0L) {
		work = 0L - work;
	}
	exp = 0L;
	while ((work & 0x80000000L) != 0x80000000L) {
		exp++;
		work <<= 1L;
	}
	exp = 130L - exp;
	work &= 0x7fffffffL;
	work >>= 8L;
	exp <<= 23L;
	return (work | exp | sign);
}

// IEEE-754 single -> q4.28
q4f fp_q4(x)
spf x;
{
	long ieee16 = 0x41800000L;	// +16.0
	long ieeen16 = 0xc1800000L;	// -16.0
	long sign;
	long exp;
	long mant;
	long result;
	if (spflt(x, ieeen16) || spfgt(x, ieee16)) return spfNAN;
	sign = x & 0x80000000L;
	exp = x & 0x7f800000L;
	exp >>= 23;
	exp -= 127L;
	mant = x & 0x007fffffL;
	mant |= 0x00800000L;
	mant <<= 5L;
	if (exp > 3L) {
		return 0L;
	} else {
		if (exp > 0L) {
			result = mant << exp;
		} else {
			result = mant >> abs(exp);
		}
		if (sign != 0L) {
			result = 0L - result;
		}
	}
	return result;
}

//--------------------------------------------------------
// Powers
//--------------------------------------------------------

// spfipow - integer power: x^y for integer x,y (y >= 0 assumed)
long spfipow(x, y)
long x;
long y;
{
	long h;
	long c;
	if (x == 0L || x == 1L || y == 1L) {
		c = x;
	} else if (x == -1L) {
		if (y % 2L == 0L) {
			c = 1L;
		} else {
			c = -1L;
		}
	} else {
		h = spfipow(x, y / 2L);
		if (y % 2L == 0L) {
			c = h * h;
		} else {
			c = h * h * x;
			printf("took odd branch:\n");
		}
	}
	return c;
}

// spfpowi - floating power to integer exponent: x^y
spf spfpowi(x, y1)
spf x;
long y1;
{
	spf h;
	long c;
	spf work;
	long y;
	

	if (y1 == 0L) return spfone;
	if (y1 == 1L) return x;
	
	y=abs(y1);

	if (spfeq(x, spfz) || spfeq(x, spfone)) return x;
	if (spfeq(x, spfn1)) {
		if (y % 2L == 0L) {
			return spfone;
		} else {
			return spfn1;
		}
	}
	h = spfpowi(x, y / 2L);
	if (y % 2L == 0L) {
		work = spfmul(h, h);
	} else {
		work = spfmul(h, h);
		work = spfmul(work, x);
	}
	if (y1<0) {
		work=spfdiv(spfone,work);
	}
	return work;
}

//--------------------------------------------------------
// spfsqr - generalized square root
//--------------------------------------------------------
spf spfsqr(x)
spf x;
{
	long sign, exp, mant, result;
	q4f mantq, rootq;
	if (spflt(x, spfz)) return spfNAN;
	if (spfeq(x, spfz)) return x;
	sign = x & 0x80000000L;
	exp = (x >> 23L) & 0x000000FFL;
	mant = x & 0x007FFFFFL;
	mant |= 0x00800000L;
	exp -= 127L;
	if (exp & 1L) {
		exp--;
		mant <<= 1L;
	}
	exp >>= 1L;
	mant = mant << 5L;
	mantq = (q4f)*(&mant);
	rootq = spfsr4(mantq);
	rootq >>= 5L;
	mant = *(&rootq);
	if (mant & 0x01000000L) {
		mant >>= 1L;
		exp++;
	}
	mant &= 0x007FFFFFL;
	result = (exp + 127L) << 23L;
	result = result | sign | mant;
	return result;
}

