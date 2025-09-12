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

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

// #define DEBUG 1

#define TANEPS		0x34000000L
#define SPFONE		0x3F800000L	// 1.0f
#define SPFPINF		0x7F800000L	// +inf
#define SPFNINF		0xFF800000L
#define EPSATN		0x00000001L
#define EPSTINY		0x33800000L	// 2^-24, pole guard

// External CORDIC atan table in Q2.62 (26 steps expected)
q2d cordtb[26];

// q2 helpers: prototypes (pointer-out, void return)
void q2asgn();
void q2add();
void q2neg();
void q2sub();
long q2cmp();
void q2dbg();

#define P2_30 0x4E800000L	// 2^30
#define P2_32 0x4F800000L	// 2^32
#define P2_54 0x5A800000L	// 2^(30+24)
#define P2_62 0x5E800000L	// 2^62

// ---- arctangent (SPF) via CORDIC vectoring ----
// optional: treat exact zeros normally
static long sra(x, n)
long x;
WORD n;
{
	WORD w;
	unsigned long ux;
	unsigned long signmask;
	w = (WORD)(8 * sizeof(long));
	ux = (unsigned long)x;
	if (n <= 0) return x;
	if (n >= w) return (x < 0) ? -1L : 0L;
	ux >>= n;
	if (x < 0) {
		signmask = ((unsigned long)~0L) << (w - n);
		ux |= signmask;
	}
	return (long)ux;
}

#define N_ATN 32
static q2d atntbl[N_ATN];
static long atn_ok = 0;
static q2d pi;
static q2d pi2;
static q2d pi2x;

// Q2.62 arctan table setup for CORDIC (i = 0..25), split into 32-bit hi/lo.
// Requires: atntbl[] is q2d (with .hi and .lo members), and atn_ok global.
void q2atni()
{
WORD	i;

	pi2.hi = 0x6487ED51L;
	pi2.lo = 0x10B4611AL;
	pi.hi = 0xC90FDAA2L;
	pi.lo = 0x9FDE7B4CL;
	pi2x.hi = 0x921FB544L;
	pi2x.lo = 0x13DEFA96L;
	
// CORDIC vectoring table: z_i = atan(2^-i), i = 0..31 (Q2.62)
atntbl[0].hi = 0x3243F6A8L;  // atan(2^-0) hi
atntbl[0].lo = 0x885A308DL;  // atan(2^-0) lo

atntbl[1].hi = 0x1DAC6705L;  // atan(2^-1) hi
atntbl[1].lo = 0x91BBCC8AL;  // atan(2^-1) lo

atntbl[2].hi = 0x0FADBAFCL;  // atan(2^-2) hi
atntbl[2].lo = 0xB83747C8L;  // atan(2^-2) lo

atntbl[3].hi = 0x07F56EA6L;  // atan(2^-3) hi
atntbl[3].lo = 0x4C4B6030L;  // atan(2^-3) lo

atntbl[4].hi = 0x03FEAB76L;  // atan(2^-4) hi
atntbl[4].lo = 0x57EB503BL;  // atan(2^-4) lo

atntbl[5].hi = 0x01FFD55BL;  // atan(2^-5) hi
atntbl[5].lo = 0xBA3762A2L;  // atan(2^-5) lo

atntbl[6].hi = 0x00FFFAABL;  // atan(2^-6) hi
atntbl[6].lo = 0x98A2C7C6L;  // atan(2^-6) lo

atntbl[7].hi = 0x007FFF55L;  // atan(2^-7) hi
atntbl[7].lo = 0x54A39398L;  // atan(2^-7) lo

atntbl[8].hi = 0x003FFFEAL;  // atan(2^-8) hi
atntbl[8].lo = 0xAAAE2302L;  // atan(2^-8) lo

atntbl[9].hi = 0x001FFFFCL;  // atan(2^-9) hi
atntbl[9].lo = 0xAAAAA96FL;  // atan(2^-9) lo

atntbl[10].hi = 0x000FFFFFL;  // atan(2^-10) hi
atntbl[10].lo = 0xAAAAADDDL;  // atan(2^-10) lo

atntbl[11].hi = 0x0007FFFFL;  // atan(2^-11) hi
atntbl[11].lo = 0xFFFF5555L;  // atan(2^-11) lo

atntbl[12].hi = 0x0003FFFFL;  // atan(2^-12) hi
atntbl[12].lo = 0xFFFFFEAAL;  // atan(2^-12) lo

atntbl[13].hi = 0x0001FFFFL;  // atan(2^-13) hi
atntbl[13].lo = 0xFFFFFFF9L;  // atan(2^-13) lo

atntbl[14].hi = 0x0000FFFFL;  // atan(2^-14) hi
atntbl[14].lo = 0xFFFFFFFEL;  // atan(2^-14) lo

atntbl[15].hi = 0x00007FFFL;  // atan(2^-15) hi
atntbl[15].lo = 0xFFFFFFFFL;  // atan(2^-15) lo

atntbl[16].hi = 0x00003FFFL;  // atan(2^-16) hi
atntbl[16].lo = 0xFFFFFFFFL;  // atan(2^-16) lo

atntbl[17].hi = 0x00001FFFL;  // atan(2^-17) hi
atntbl[17].lo = 0xFFFFFFFFL;  // atan(2^-17) lo

atntbl[18].hi = 0x00000FFFL;  // atan(2^-18) hi
atntbl[18].lo = 0xFFFFFFFFL;  // atan(2^-18) lo

atntbl[19].hi = 0x000007FFL;  // atan(2^-19) hi
atntbl[19].lo = 0xFFFFFFFFL;  // atan(2^-19) lo

atntbl[20].hi = 0x000003FFL;  // atan(2^-20) hi
atntbl[20].lo = 0xFFFFFFFFL;  // atan(2^-20) lo

atntbl[21].hi = 0x000001FFL;  // atan(2^-21) hi
atntbl[21].lo = 0xFFFFFFFFL;  // atan(2^-21) lo

atntbl[22].hi = 0x000000FFL;  // atan(2^-22) hi
atntbl[22].lo = 0xFFFFFFFFL;  // atan(2^-22) lo

atntbl[23].hi = 0x0000007FL;  // atan(2^-23) hi
atntbl[23].lo = 0xFFFFFFFFL;  // atan(2^-23) lo

atntbl[24].hi = 0x0000003FL;  // atan(2^-24) hi
atntbl[24].lo = 0xFFFFFFFFL;  // atan(2^-24) lo

atntbl[25].hi = 0x0000001FL;  // atan(2^-25) hi
atntbl[25].lo = 0xFFFFFFFFL;  // atan(2^-25) lo

atntbl[26].hi = 0x0000000FL;  // atan(2^-26) hi
atntbl[26].lo = 0xFFFFFFFFL;  // atan(2^-26) lo

atntbl[27].hi = 0x00000007L;  // atan(2^-27) hi
atntbl[27].lo = 0xFFFFFFFFL;  // atan(2^-27) lo

atntbl[28].hi = 0x00000003L;  // atan(2^-28) hi
atntbl[28].lo = 0xFFFFFFFFL;  // atan(2^-28) lo

atntbl[29].hi = 0x00000001L;  // atan(2^-29) hi
atntbl[29].lo = 0xFFFFFFFFL;  // atan(2^-29) lo

atntbl[30].hi = 0x00000000L;  // atan(2^-30) hi
atntbl[30].lo = 0xFFFFFFFFL;  // atan(2^-30) lo

atntbl[31].hi = 0x00000000L;  // atan(2^-31) hi
atntbl[31].lo = 0x7FFFFFFFL;  // atan(2^-31) lo

	atn_ok = 1;
	
#ifdef DEBUG	
	printf("i____atntbl.hi atntbl.lo\n");
	
	for(i=0;i<32;i++) {
	   printf("%3d   %08lX  %08lX\n",i,atntbl[i].hi,atntbl[i].lo);
	}
#endif
}

// arctangent via CORDIC vectoring mode in q2.30
spf spfatn(a)
spf a;
{
        q2d z;
        q2f x;
        q2f y;
        q2f xi;
        long i;
        long neg;
        long swp;

        if (spfeq(a, spfz)) return spfz;
        if (!atn_ok) q2atni();

        /* record sign and work on |a| */
        neg = 0L;
        if (spflt(a, spfz)) {
                a = spfneg(a);
                neg = 1L;
        }

        /* keep slope |y/x| <= 1 in Q2.30 to avoid overflow */
        if (spfge(a, spfone)) {
                // swap and y
                y   = fp_q2(spfdiv(spfone,a));
                x   = 0x40000000L;      /* 1.0 in Q2.30 */
                swp = 1L;
        } else {
                /* y = a, x = 1 */
                y   = fp_q2(a);
                x   = 0x40000000L;      /* 1.0 in Q2.30 */
                swp = 0L;
        }

	// scale x and y to prevent overflow at 2!  Maintain ratio.
	// we have to scale if tan value is for over 30 degrees.
        if (a>TAN_30) {
            x=sra(x,1);
            y=sra(y,1);
        }


        z.hi = 0L;
        z.lo = 0L;

        for (i = 0L; i < N_ATN; ++i) {
                if (y >  0L) {
                        xi = x;
                        x  = x + sra(y, (WORD)i);
                        y  = y - sra(xi, (WORD)i);
                        q2add(&z, &z, &atntbl[i]);
                } else {
                        xi = x;
                        x  = x - sra(y, (WORD)i);
                        y  = y + sra(xi, (WORD)i);
                        q2sub(&z, &z, &atntbl[i]);
                }
        }

        if (swp) {
                /* z = pi/2 - z  (since we used 1/a) */
                q2sub(&z, &pi2, &z);
        }

        if (neg) q2neg(&z, &z);

        return q2d_fp(&z);
}

#define TINY	0x3e000000L	// 0.125
#define C_1_2	0x3f000000L
#define C_1_6	0x3e2aaaabL
#define C_1_24	0x3d2aaaabL
#define C_1_120	0x3c088889L

// spfshc: compute sinh and cosh together
void spfshc(x, ps, pc)
spf x;
spf *ps;
spf *pc;
{
	spf a;
	spf s;
	spf c;
	spf e;
	spf ei;
	spf t;
	spf x2;
	spf x3;
	spf x4;
	spf x5;
	a = spfabs(x);
	if (a < (spf)TINY) {
		x2 = spfmul(a, a);
		x3 = spfmul(x2, a);
		x4 = spfmul(x2, x2);
		x5 = spfmul(x3, x2);
		c = spfone;
		t = spfmul(x2, (spf)C_1_2);
		c = spfadd(c, t);
		t = spfmul(x4, (spf)C_1_24);
		c = spfadd(c, t);
		s = a;
		t = spfmul(x3, (spf)C_1_6);
		s = spfadd(s, t);
		t = spfmul(x5, (spf)C_1_120);
		s = spfadd(s, t);
	} else {
		e = spfexp(a);
		ei = spfdiv(spfone, e);
		s = spfsub(e, ei);
		s = spfmul(s, spfhlf);
		c = spfadd(e, ei);
		c = spfmul(c, spfhlf);
	}
	if ((long)x < 0L) s = spfneg(s);
	*ps = s;
	*pc = c;
}

/* return both: *ps = sinh(x), *pc = cosh(x) */
void hypair(x, ps, pc)
spf x;
spf *ps;
spf *pc;
{
	/* constants (IEEE-754 single in hex) */
#define HLF     0x3F000000L  /* 0.5f */
#define TSMALL  0x39800000L  /* ~2^-12: small-x threshold */
#define TBIG    0x42B17218L  /* ~88.72: ln(FLT_MAX) */

	spf ax, ex, rinv, s0, c0, x2, t;
	long neg;

	ax  = spfabs(x);
	neg = (x & 0x80000000L) ? 1L : 0L;

	/* very small |x|: cheap series to avoid cancellation */
	if (spfcmp(ax, TSMALL) <= 0L) {
		x2 = spfmul(x, x);
		/* cosh ≈ 1 + x^2/2 */
		*pc = spfadd(spfone, spfmul(HLF, x2));
		/* sinh ≈ x + x^3/6  (1/6 = 0x3D2AAAABL) */
		t   = spfmul(x2, 0x3D2AAAABL);
		*ps = spfadd(x, spfmul(x, t));
		return;
	}

	/* normal range: two-exponent form with correct +/- */
	if (spfcmp(ax, TBIG) < 0L) {
		ex   = spfexp(ax);
		rinv = spfdiv(spfone, ex);
		c0   = spfmul(HLF, spfadd(ex, rinv)); /* ½(e^|x| + e^-|x|) */
		s0   = spfmul(HLF, spfsub(ex, rinv)); /* ½(e^|x| − e^-|x|) */
		if (neg) s0 = spfneg(s0);
		*ps = s0;
		*pc = c0;
		return;
	}

	/* very large |x|: e^-|x| underflows ⇒ cosh ≈ ½ e^|x|, sinh ≈ ±cosh */
	ex = spfexp(spfmul(ax, HLF));         /* e^{|x|/2} */
	c0 = spfmul(HLF, spfmul(ex, ex));     /* ½ e^{|x|}  (via square) */
	s0 = neg ? spfneg(c0) : c0;
	*ps = s0;
	*pc = c0;
}


spf spfsnh(x) 
spf x;
{
 spf s,c; 
	hypair(x,&s,&c); 
	return s; 
}

spf spfcoh(x) 
spf x;
{ 
spf s,c; 
	hypair(x,&s,&c); 
	return c; 
}

spf spftnh(x)
spf x; 
{ 
spf s,c; 
	hypair(x,&s,&c); 
	return spfdiv(s,c); 
}

// asnh(x) = ln( x + sqrt(x*x + 1) )   (odd function)
spf asnh(x)
spf x;
{
    spf t, s;

    if (spfeq(x, spfz)) return spfz;            // exact zero

    t = spfmul(x, x);                            // x*x
    t = spfadd(t, spfone);                       // x*x + 1
    s = spfsqr(t);                               // sqrt(x*x + 1)
    return spfln(spfadd(x, s));                  // ln(x + sqrt(...))
}

// acnh(x) = ln( x + sqrt((x-1)*(x+1)) ),  domain x >= 1
spf acnh(x)
spf x;
{
    spf xm1, xp1, rad, s;

    if (spflt(x, spfone)) return spfNAN;         // domain guard

    xm1 = spfsub(x, spfone);                     // x - 1
    xp1 = spfadd(x, spfone);                     // x + 1
    rad = spfmul(xm1, xp1);                      // (x-1)*(x+1) = x*x - 1
    s   = spfsqr(rad);                           // sqrt(x*x - 1)
    return spfln(spfadd(x, s));                  // ln(x + sqrt(...))
}

// atnh(x) = 0.5 * ln( (1+x)/(1-x) ),  domain |x| < 1
spf atnh(x)
spf x;
{
    spf ax, num, den, q, l;

    ax = spfabs(x);
    if (spfcmp(ax, spfone) >= 0L) return spfNAN; // |x|>=1 out of domain

    num = spfadd(spfone, x);                     // 1 + x
    den = spfsub(spfone, x);                     // 1 - x
    q   = spfdiv(num, den);
    l   = spfln(q);
    return spfmul(spfhlf, l);                    // 0.5 * ln(...)
}


// spfa2: atan2(y,x) — angle in [-pi, +pi]
spf spfa2(y, x)
spf y;
spf x;
{
	spf z;
	spf a;
	spf r;
	if ((long)x > 0L) {
		z = spfdiv(y, x);
		r = spfatn(z);
		return r;
	}
	if ((long)x < 0L && !((long)y < 0L)) {
		z = spfdiv(y, x);
		a = spfatn(z);
		r = spfadd(a, spfpi);
		return r;
	}
	if ((long)x < 0L && (long)y < 0L) {
		z = spfdiv(y, x);
		a = spfatn(z);
		r = spfsub(a, spfpi);
		return r;
	}
	if ((long)x == 0L) {
		if ((long)y > 0L) return spfpi2;
		if ((long)y < 0L) return spfneg(spfpi2);
		return 0L;
	}
	return 0L;
}

// spfhyp: hypot(a,b) = sqrt(a^2 + b^2) stably
spf spfhyp(a, b)
spf a;
spf b;
{
	spf ua;
	spf ub;
	spf u;
	spf v;
	spf t;
	spf v_u;
	spf v_u2;
	spf s;
	spf r;
	ua = spfabs(a);
	ub = spfabs(b);
	if (ua > ub) {
		u = ua;
		v = ub;
	} else {
		u = ub;
		v = ua;
	}
	if ((long)u == 0L) return 0L;
	v_u = spfdiv(v, u);
	v_u2 = spfmul(v_u, v_u);
	t = spfadd(spfone, v_u2);
	s = spfsqr(t);
	r = spfmul(u, s);
	return r;
}

// spfmdf: modf(x,&ip) splits x into integer part ip and frac
// Here: ip = trunc(x) (toward zero)
spf spfmdf(x, pip)
spf x;
spf *pip;
{
	spf ip;
	spf frac;
	if ((long)x >= 0L) {
		ip = spfflr(x);
	} else {
		ip = spfcl(x);
	}
	*pip = ip;
	frac = spfsub(x, ip);
	return frac;
}

// spffxp: frexp(x,&m,&e)  with x = m * 2^e, 0.5 <= |m| < 1 (or 0)
void spffxp(x, pm, pe)
spf x;
spf *pm;
long *pe;
{
	unsigned long u;
	unsigned long sign;
	unsigned long exp;
	unsigned long frac;
	long e;
	long s;
	unsigned long m_bits;
	u = (unsigned long)x;
	sign = u & (unsigned long)0x80000000L;
	exp = (u >> 23) & 0xFFL;
	frac = u & (unsigned long)0x7FFFFFL;
	if (exp == (unsigned long)0L && frac == (unsigned long)0L) {
		*pm = x;
		*pe = 0L;
		return;
	}
	if (exp != (unsigned long)0L) {
		e = (long)exp - 126L;
		m_bits = sign | (((unsigned long)126L) << 23) | frac;
		*pm = (spf)m_bits;
		*pe = e;
		return;
	}
	s = 0L;
	while ((frac & (unsigned long)0x00400000L) == (unsigned long)0L) {
		frac <<= 1;
		s = s + 1;
		if (s > 23) break;
	}
	frac &= (unsigned long)0x007FFFFFL;
	e = -125L - s;
	m_bits = sign | (((unsigned long)126L) << 23) | frac;
	*pm = (spf)m_bits;
	*pe = e;
}

// spfldp: ldexp(m,e) -> m * 2^e  (assumes |m| in [0.5,1) or 0)
// 6-char name wrapper (since you’re keeping names ≤ 6 chars)
spf spfldp(x, n)
spf x; long n;
{
    return spfscb(x, n);   // delegate to your working scalbn
}


// spfcps: copysign(x,y) — return x with sign of y
spf spfcps(x, y)
spf x;
spf y;
{
	unsigned long ux;
	unsigned long uy;
	unsigned long out;
	ux = (unsigned long)x;
	uy = (unsigned long)y;
	out = (ux & (unsigned long)0x7FFFFFFFL) | (uy & (unsigned long)0x80000000L);
	return (spf)out;
}

// spfscb: scalbn(x,n) -> x * 2^n  (fast exponent-field scaling)
spf spfscb(x, n)
spf x;
long n;
{
	spf r;
	unsigned long u;
	unsigned long sign;
	unsigned long exp;
	unsigned long frac;
	long e;
	long k;
	long steps;
	u = (unsigned long)x;
	if ((u & (unsigned long)0x7FFFFFFFL) == (unsigned long)0L) return x;
	sign = u & (unsigned long)0x80000000L;
	exp = (u >> 23) & (unsigned long)0xFFL;
	frac = u & (unsigned long)0x7FFFFFL;
	if (exp == (unsigned long)0L) {
		r = x;
		if (n > 0) {
			k = 0;
			while (k < n) {
				r = spfmul(r, spftwo);
				k = k + 1;
			}
		} else if (n < 0) {
			k = 0;
			while (k < -n) {
				r = spfmul(r, spfhlf);
				k = k + 1;
			}
		}
		return r;
	}
	e = (long)exp + n;
	if (e <= 0L) {
		r = x;
		if (n < 0) {
			steps = -(n + (long)exp - 1L);
			if (steps < 0) steps = 0;
			k = 0;
			while (k < steps) {
				r = spfmul(r, spfhlf);
				k = k + 1;
			}
			return r;
		} else {
			return r;
		}
	}
	if (e >= 255L) {
		r = x;
		k = 0;
		while (k < n) {
			r = spfmul(r, spftwo);
			k = k + 1;
		}
		return r;
	}
	u = sign | ((unsigned long)e << 23) | frac;
	return (spf)u;
}

