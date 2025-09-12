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
// Title      : spfefn.c
// Written by : J. Lovrinic
// Date       : 20241028
//--------------------------------------------------------
// Description: Euler Functions
//--------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

#define spf_15	0x3e19999aL
#define spf_48	0x3ef5c28fL
#define spf_2	0x40000000L
#define spf_3	0x40400000L
#define spf_4	0x40800000L
#define spf_5	0x40A00000L
#define spf_6	0x40C00000L
#define spf_7	0x40E00000L

// exp(x) — return e^x where 0 < x < 1
spf exp(x)
spf x;
{
	spf t;
	t = spfadd(spfone, spfdiv(x, spf_7));
	t = spfadd(spfone, spfmul(spfdiv(x, spf_6), t));
	t = spfadd(spfone, spfmul(spfdiv(x, spf_5), t));
	t = spfadd(spfone, spfmul(spfdiv(x, spf_4), t));
	t = spfadd(spfone, spfmul(spfdiv(x, spf_3), t));
	t = spfadd(spfone, spfmul(spfdiv(x, spf_2), t));
	t = spfadd(spfone, spfmul(x, t));
	return t;
}

// spfexp — general-purpose e^x for floating point values
spf spfexp(val)
spf val;
{
	long i;
	q4f frac, wfq4;
	spf work;
	spf workf;
	spf j;
	work = spfabs(val);
	if (spfeq(val, spfz)) {
		return spfone;
	} else if (spfeq(val, spfone)) {
		work = spfe;
	} else {
		workf = spfint(work);
		i = spflng(workf);
		workf = spfsub(work, workf);
		workf = exp(workf);
		work = spfpowi(spfe, i);
		work = spfmul(work, workf);
	}
	if (spflt(val,spfz)) {
		work = spfdiv(spfone, work);
	}
	return work;
}

// logn — natural logarithm using atanh series on (r/(2+r))
q4f logn(x)
q4f x;
{
	spf r, z, z2, term, sum;
	spf result;

#define F_2	0x40000000L
#define F_3	0x40400000L
#define F_5	0x40A00000L
#define F_7	0x40E00000L
#define F_9	0x41100000L
#define F_11	0x41300000L
#define F_13	0x41500000L

	// r = x - 1
	r = spfsub(x, spfone);
	// z = r / (2 + r)
	z = spfdiv(r, spfadd(F_2, r));
	// start with sum = z
	sum = z;
	term = z;
	// z^3 / 3
	z2 = spfmul(z, z);
	term = spfmul(term, z2);
	sum = spfadd(sum, spfdiv(term, F_3));
	// z^5 / 5
	term = spfmul(term, z2);
	sum = spfadd(sum, spfdiv(term, F_5));
	// z^7 / 7
	term = spfmul(term, z2);
	sum = spfadd(sum, spfdiv(term, F_7));
	// z^9 / 9
	term = spfmul(term, z2);
	sum = spfadd(sum, spfdiv(term, F_9));
	// z^11 / 11
	term = spfmul(term, z2);
	sum = spfadd(sum, spfdiv(term, F_11));
	// z^13 / 13
	term = spfmul(term, z2);
	sum = spfadd(sum, spfdiv(term, F_13));
	result = spfadd(sum, sum);
	return result;
}

spf spfln(val)
spf val;
{
    unsigned long w1;
    long e;
    spf expf, ln2_exp, mant, ln_m, result;

    if (spfle(val, spfz)) return spfNAN;   /* x <= 0 -> NaN */

    /* bit extract */
    w1 = (unsigned long)val;

    /* unbiased exponent e = E - 127  */
    e = (long)((w1 & (unsigned long) 0x7F800000L) >> 23) - 127L;
    expf = spfltf(e);
    ln2_exp = spfmul(expf, spfl2);         /* e * ln(2) */

    /* mantissa in [1,2): put hidden 1 back, force exponent=127 */
    mant = (spf)((w1 & (unsigned long) 0x007FFFFFL) | (unsigned long) 0x3F800000L);

    if (spfeq(mant, spfone)) {
        ln_m = spfz;
    } else {
        ln_m = logn(mant);                  /* series on [1,2) */
    }

    result = spfadd(ln2_exp, ln_m);
    return result;
}


spf spflog(val)
spf val;
{
	spf wf;
	wf = spfln(val);
	wf = spfdiv(wf, spfl10);
	return wf;
}

spf spflg2(val)
spf val;
{
	spf wf;
	wf = spfln(val);
	wf = spfdiv(wf, spfl2);
	return wf;
}

spf spfpow(x, y)
spf x;
spf y;
{
	spf result;
	result = spfln(x);
	result = spfmul(result, y);
	result = spfexp(result);
	return result;
}

