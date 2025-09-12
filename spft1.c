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

//#define DEBUG 1

#define TANEPS		0x34000000L
#define SPFONE		0x3F800000L	// 1.0f
#define SPFPINF		0x7F800000L	// +inf
#define SPFNINF		0xFF800000L
#define EPSATN		0x00000001L
#define EPSTINY		0x33800000L	// 2^-24, pole guard

// angle/ratio locks (IEEE-754 single bit patterns)
#define SPF_PI6     0x3F060A92L  // pi/6  ≈ 0.5235988
#define SPF_PI4     0x3F490FD8L  // pi/4  ≈ 0.7853982
#define SPF_PI3     0x3F86C16CL  // pi/3  ≈ 1.0471976
#define SPF_PI2     0x3FC90FDBL  // pi/2  ≈ 1.5707963
#define SPF_RT2_2   0x3F3504F3L  // sqrt(2)/2
#define SPF_RT3_2   0x3F5DB3D7L  // sqrt(3)/2
#define LOCK_EPS    0x358637BDL  // ~1e-6 (radians)

/* file-private CORDIC atan table */
#define NCORDTB	40
#define NCORD	26
static q2d cordtb[NCORDTB];
static long cord_ok = 0L;

// q2 helpers: prototypes (pointer-out, void return)
void q2asgn();
void q2add();
void q2neg();
void q2sub();
long q2cmp();
void q2dbg();

// reducer we’re using now
spf octred();

// CORDIC kernel
void cord8();

#define P2_30 0x4E800000L	// 2^30
#define P2_32 0x4F800000L	// 2^32
#define P2_54 0x5A800000L	// 2^(30+24)
#define P2_62 0x5E800000L	// 2^62

// Return sign of an SPF number as +1L or -1L
long spfsgn(x)
spf x;
{
	if (x & 0x80000000L) return -1L;
	return 1L;
}


// ---------------- q2d helpers ----------------
void q2asgn(x, y)
q2d *x;
q2d *y;
{
	x->hi = y->hi;
	x->lo = y->lo;
}

// r = a + b
void q2add(r, a, b)
q2d *r;
q2d *a;
q2d *b;
{
    unsigned long u1;
    unsigned long u2;
    unsigned long sum;

    u1 = (unsigned long)a->lo;
    u2 = (unsigned long)b->lo;
    sum = u1 + u2;

    r->lo = (long)sum;
    r->hi = a->hi + b->hi;

    if (sum < u1)
        r->hi = r->hi + 1L;
}

// r = -a
void q2neg(r, a)
q2d *r;
q2d *a;
{
    unsigned long a_lo;
    unsigned long n_lo;
    long a_hi;
    long n_hi;

    a_lo = (unsigned long)a->lo;
    a_hi = a->hi;

    n_lo = ~a_lo + 1L;
    n_hi = ~a_hi;

    if (n_lo == 0L)
        n_hi = n_hi + 1L;

    r->lo = (long)n_lo;
    r->hi = n_hi;
}

// r = a - b
void q2sub(r, a, b)
q2d *r;
q2d *a;
q2d *b;
{
    unsigned long a_lo;
    unsigned long b_lo;
    unsigned long diff_lo;
    long a_hi;
    long b_hi;
    long diff_hi;

    a_lo = (unsigned long)a->lo;
    b_lo = (unsigned long)b->lo;
    diff_lo = a_lo - b_lo;

    a_hi = a->hi;
    b_hi = b->hi;
    diff_hi = a_hi - b_hi;

    if (a_lo < b_lo)
        diff_hi = diff_hi - 1L;

    r->lo = (long)diff_lo;
    r->hi = diff_hi;
}


// r = a >> n (arithmetic) with 0<=n<=63
void q2shr(r, a, n)
q2d *r;
q2d *a;
long n;
{
	unsigned long ulo;
	long shi;
	if (n <= 0L) {
		q2asgn(r, a);
		return;
	}
	if (n >= 63L) {
		r->lo = (a->hi < 0L) ? 0xFFFFFFFFL : 0x00000000L;
		r->hi = (a->hi < 0L) ? 0xFFFFFFFFL : 0x00000000L;
		return;
	}
	if (n >= 32L) {
		shi = a->hi;
		if (n == 32L) {
			r->lo = (unsigned long)shi;
		} else {
			r->lo = (unsigned long)((unsigned long)shi >> (n - 32L));
		}
		r->hi = (shi < 0L) ? 0xFFFFFFFFL : 0x00000000L;
		return;
	}
	// 0..31
	ulo = (unsigned long)a->lo;
	r->lo = (unsigned long)(ulo >> n);
	r->hi = a->hi >> n;
	if (n != 0L) r->lo = r->lo | (unsigned long)((unsigned long)a->hi << (32L - n));
}

// compare
long q2cmp(a, b)
q2d *a;
q2d *b;
{
	if (a->hi < b->hi) return -1L;
	if (a->hi > b->hi) return 1L;
	if ((unsigned long)a->lo < (unsigned long)b->lo) return -1L;
	if ((unsigned long)a->lo > (unsigned long)b->lo) return 1L;
	return 0L;
}

// debug
void q2dbg(lbl, v)
char *lbl;
q2d *v;
{
	printf("%s: hi=0x%08lX lo=0x%08lX\n", lbl, (unsigned long)v->hi, (unsigned long)v->lo);
}

// ------------- SPF <-> Q2.62 -------------
// r = a (spf -> q2.62) : uses 2^30 split + 2^32 for low 32 frac
void fpq2d(r, a)
q2d *r;
spf a;
{
	long neg;
	spf t;
	spf ai;
	spf fr;
	long hi;
	long lo;
	q2d tmp;
	neg = 0L;
	if (a & 0x80000000L) {
		a = spfneg(a);
		neg = 1L;
	}
	t = spfmul(a, P2_30);
	ai = spfint(t);
	hi = spflng(ai);
	r->hi = hi;
	fr = spfsub(t, spfltf(hi));
	fr = spfmul(fr, P2_32);
	lo = spflng(fr);
	r->lo = (long)((unsigned long)lo);
	if (neg) {
		q2neg(&tmp, r);
		q2asgn(r, &tmp);
	}
#ifdef DEBUG
	printf("fpq2d: a=0x%08lX -> hi=0x%08lX lo=0x%08lX\n", (unsigned long)a, (unsigned long)r->hi, (unsigned long)r->lo);
#endif
}

// spf = v (q2.62 -> spf) : value = hi/2^30 + lo/2^62
// Convert Q2.62 (hi:lo) to IEEE-754 single.
// We round the 64-bit fixed value to Q2.30 using
// round-to-nearest, ties-to-even, then call q2_fp().
spf q2d_fp(z)
q2d *z;
{
	q2f h;
	unsigned long u;
#ifdef DEBUG
	printf("q2d_fp incoming z: %08lX %08lX\n", (unsigned long)z->hi, (unsigned long)z->lo);
#endif
	h = z->hi;
	u = (unsigned long)z->lo;
	// round-to-nearest, ties-to-even on the discarded 32 bits
	if (u > (unsigned long)0x80000000L) {
		h += 1L;
	} else if (u == (unsigned long)0x80000000L) {
		if ((h & 1L) != 0L) h += 1L;
	}
	return q2_fp(h);
}

// Load atan(2^-i) table in Q2.62 intocordtb[0..25]
// NOTE: indices 26..40 below exceed cordtb[26]; keep table size consistent if you use them.
void cordin()
{
	cord_ok=1L;
	
	cordtb[0].hi = 0x3243F6A8L; cordtb[0].lo = 0x885A3000L;	// atan(2^-0)  = π/4
	cordtb[1].hi = 0x1DAC6705L; cordtb[1].lo = 0x61BB4F00L;	// atan(2^-1)
	cordtb[2].hi = 0x0FADBAFCL; cordtb[2].lo = 0x96406E80L;	// atan(2^-2)
	cordtb[3].hi = 0x07F56EA6L; cordtb[3].lo = 0xAB0BDB80L;	// atan(2^-3)
	cordtb[4].hi = 0x03FEAB76L; cordtb[4].lo = 0xE59FBD40L;	// atan(2^-4)
	cordtb[5].hi = 0x01FFD55BL; cordtb[5].lo = 0xBA976250L;	// atan(2^-5)
	cordtb[6].hi = 0x00FFFAAAL; cordtb[6].lo = 0xDDDB94D8L;	// atan(2^-6)
	cordtb[7].hi = 0x007FFF55L; cordtb[7].lo = 0x56EEEA5CL;	// atan(2^-7)
	cordtb[8].hi = 0x003FFFEAL; cordtb[8].lo = 0xAAB7776EL;	// atan(2^-8)
	cordtb[9].hi = 0x001FFFFDL; cordtb[9].lo = 0x5555BBBCL;	// atan(2^-9)
	cordtb[10].hi = 0x000FFFFFL; cordtb[10].lo = 0xAAAAADDEL;	// atan(2^-10)
	cordtb[11].hi = 0x0007FFFFL; cordtb[11].lo = 0xF555556FL;	// atan(2^-11)
	cordtb[12].hi = 0x0003FFFFL; cordtb[12].lo = 0xFEAAAAACL;	// atan(2^-12)
	cordtb[13].hi = 0x0001FFFFL; cordtb[13].lo = 0xFFD55555L;	// atan(2^-13)
	cordtb[14].hi = 0x0000FFFFL; cordtb[14].lo = 0xFFFAAAABL;	// atan(2^-14)
	cordtb[15].hi = 0x00007FFFL; cordtb[15].lo = 0xFFFF5555L;	// atan(2^-15)
	cordtb[16].hi = 0x00003FFFL; cordtb[16].lo = 0xFFFFEAABL;	// atan(2^-16)
	cordtb[17].hi = 0x00001FFFL; cordtb[17].lo = 0xFFFFFD55L;	// atan(2^-17)
	cordtb[18].hi = 0x00000FFFL; cordtb[18].lo = 0xFFFFFFABL;	// atan(2^-18)
	cordtb[19].hi = 0x000007FFL; cordtb[19].lo = 0xFFFFFFF5L;	// atan(2^-19)
	cordtb[20].hi = 0x000003FFL; cordtb[20].lo = 0xFFFFFFFEL;	// atan(2^-20)
	cordtb[21].hi = 0x000001FFL; cordtb[21].lo = 0xFFFFFFFFL;	// atan(2^-21)
	cordtb[22].hi = 0x000000FFL; cordtb[22].lo = 0xFFFFFFFFL;	// atan(2^-22)
	cordtb[23].hi = 0x0000007FL; cordtb[23].lo = 0xFFFFFFFFL;	// atan(2^-23)
	cordtb[24].hi = 0x0000003FL; cordtb[24].lo = 0xFFFFFFFFL;	// atan(2^-24)
	cordtb[25].hi = 0x0000001FL; cordtb[25].lo = 0xFFFFFFFFL;	// atan(2^-25)

	// The following entries are plain 2^-k scalars, not atan angles.
	// They are unused by cord8() (loops 26 steps), but kept here if you expand iterations.
	cordtb[26].hi = 0x00000010L; cordtb[26].lo = 0x00000000L;	// 2^-26
	cordtb[27].hi = 0x00000008L; cordtb[27].lo = 0x00000000L;	// 2^-27
	cordtb[28].hi = 0x00000004L; cordtb[28].lo = 0x00000000L;	// 2^-28
	cordtb[29].hi = 0x00000002L; cordtb[29].lo = 0x00000000L;	// 2^-29
	cordtb[30].hi = 0x00000001L; cordtb[30].lo = 0x00000000L;	// 2^-30
	cordtb[31].hi = 0x00000000L; cordtb[31].lo = 0x80000000L;	// 2^-31
	cordtb[32].hi = 0x00000000L; cordtb[32].lo = 0x40000000L;	// 2^-32
	cordtb[33].hi = 0x00000000L; cordtb[33].lo = 0x20000000L;	// 2^-33
	cordtb[34].hi = 0x00000000L; cordtb[34].lo = 0x10000000L;	// 2^-34
	cordtb[35].hi = 0x00000000L; cordtb[35].lo = 0x08000000L;	// 2^-35
	cordtb[36].hi = 0x00000000L; cordtb[36].lo = 0x04000000L;	// 2^-36
	cordtb[37].hi = 0x00000000L; cordtb[37].lo = 0x02000000L;	// 2^-37
	cordtb[38].hi = 0x00000000L; cordtb[38].lo = 0x01000000L;	// 2^-38
	cordtb[39].hi = 0x00000000L; cordtb[39].lo = 0x00800000L;	// 2^-39
}

// ------------- CORDIC core (0..pi/8) -------------
void cord8(a, sp, cp)
spf a;
spf *sp;
spf *cp;
{
	q2d x;
	q2d y;
	q2d z;
	q2d xn;
	q2d yn;
	q2d t1;
	q2d t2;
	WORD i;

	fpq2d(&z, a);
	x.hi = 0x26DD3B6AL;
	x.lo = 0x10D7969AL;
	y.hi = 0x00000000L;
	y.lo = 0x00000000L;
	for (i = 0; i < NCORD; i++) {
		q2asgn(&xn, &x);
		q2asgn(&yn, &y);
		if (z.hi < 0L) {
			q2shr(&t1, &yn, (long)i);
			q2add(&x, &xn, &t1);
			q2shr(&t2, &xn, (long)i);
			q2sub(&y, &yn, &t2);
			q2add(&z, &z, &cordtb[i]);
		} else {
			q2shr(&t1, &yn, (long)i);
			q2sub(&x, &xn, &t1);
			q2shr(&t2, &xn, (long)i);
			q2add(&y, &yn, &t2);
			q2sub(&z, &z, &cordtb[i]);
		}
#ifdef DEBUG
		printf("iter %2d: x=0x%08lX%08lX y=0x%08lX%08lX z=0x%08lX%08lX\n",
		       (int)i,
		       (unsigned long)x.hi, (unsigned long)x.lo,
		       (unsigned long)y.hi, (unsigned long)y.lo,
		       (unsigned long)z.hi, (unsigned long)z.lo);
#endif
	}
	*cp = q2d_fp(&x);
	*sp = q2d_fp(&y);
}

// Reduce an angle to the first octant [0, π/4] and return octant index (0..7)
// a  : input angle (radians, SPF)
// *poct : output octant number
// ret: reduced angle in [0, π/4]
/* octred - reduce angle to [0, pi/4]; return octant 0..7 via *poct */
/* octred - reduce angle to [0, pi/4]; return octant 0..7 via *poct */
// ret: reduced angle in [0, π/4]
spf octred(a, poct)
spf a;
long *poct;
{
    spf pi     = 0x40490FDBL;  /* π */
    spf pio2   = 0x3FC90FD8L;  /* π/2 */
    spf twopi  = spfadd(pi, pi);
    spf t, q;
    long oct;
    WORD w;

    /* wrap a into [0, 2π) */
    while (spfcmp(a, 0x00000000L) < 0L)  a = spfadd(a, twopi);
    while (spfcmp(a, twopi)       >= 0L) a = spfsub(a, twopi);

    /* octant index: floor( (a/(π/2)) * 2 ) */
    t   = spfmul(spfdiv(a, pio2), 0x40000000L); /* *2.0f */
    q   = spfint(t);
    oct = spflng(q);

    /* Clamp to 0..7; if it rounded to 8 (i.e., a==2π), treat as 0 with angle 0 */
    if (oct < 0L) oct = 0L;
    if (oct > 7L) {
        *poct = 0L;
        return spfz;  /* reduced angle = 0 */
    }
    *poct = oct;
    w = (WORD)oct;

    /* map into first octant [0, π/4] */
    if (w == 0) return a;                          /* [0,   π/4)  */
    if (w == 1) return spfsub(pio2, a);            /* [π/4, π/2)  */
    if (w == 2) return spfsub(a, pio2);            /* [π/2, 3π/4) */
    if (w == 3) return spfsub(pi, a);              /* [3π/4, π)   */
    if (w == 4) return spfsub(a, pi);              /* [π,   5π/4) */
    {
        spf thrp2 = spfadd(pi, pio2);              /* 3π/2 */
        if (w == 5) return spfsub(thrp2, a);       /* [5π/4, 3π/2) */
        if (w == 6) return spfsub(a, thrp2);       /* [3π/2, 7π/4) */
    }
    /* w == 7 */
    return spfsub(twopi, a);                       /* [7π/4, 2π) */
}




// map base (s0,c0) from first-octant back to full angle using oct
static void octmap(ps, pc, s0, c0, oct)
spf *ps;
spf *pc;
spf s0;
spf c0;
long oct;
{
	WORD w;
	w = (WORD)oct;
	switch (w) {
	case 0:  *ps = s0;             *pc = c0;             break;
	case 1:  *ps = c0;             *pc = s0;             break;
	case 2:  *ps = c0;             *pc = spfneg(s0);     break;
	case 3:  *ps = s0;             *pc = spfneg(c0);     break;
	case 4:  *ps = spfneg(s0);     *pc = spfneg(c0);     break;
	case 5:  *ps = spfneg(c0);     *pc = spfneg(s0);     break;
	case 6:  *ps = spfneg(c0);     *pc = s0;             break;
	case 7:  *ps = spfneg(s0);     *pc = c0;             break;
	default: *ps = 0x7FC00000L;    *pc = 0x7FC00000L;    break;
	}
}

// lock inside reduced domain: 0, pi/6, pi/4
// returns 1 if locked (ps/pc set), else 0
static long locksp(ang, oct, ps, pc)
spf ang;
long oct;
spf *ps;
spf *pc;
{
	spf d0;
	spf d30;
	spf d45;
	spf s0;
	spf c0;

	d0  = spfabs(ang);
	if (spfle(d0, LOCK_EPS)) {
		s0 = spfz;
		c0 = spfone;
		octmap(ps, pc, s0, c0, oct);
		return 1L;
	}

	d30 = spfabs(spfsub(ang, SPF_PI6));
	if (spfle(d30, LOCK_EPS)) {
		s0 = spfhlf;
		c0 = (spf)SPF_RT3_2;
		octmap(ps, pc, s0, c0, oct);
		return 1L;
	}

	d45 = spfabs(spfsub(ang, SPF_PI4));
	if (spfle(d45, LOCK_EPS)) {
		s0 = (spf)SPF_RT2_2;
		c0 = (spf)SPF_RT2_2;
		octmap(ps, pc, s0, c0, oct);
		return 1L;
	}

	return 0L;
}

// ------------- public trig -------------
spf spfsin(a)
spf a;
{
	spf ang;
	spf s0;
	spf c0;
	spf s;
	spf c;
	long oct;

	if(!cord_ok) cordin();
	ang = octred(a, &oct);
	if (locksp(ang, oct, &s0, &c0)) return s0;
	cord8(ang, &s, &c);
	octmap(&s0, &c0, s, c, oct);
	return s0;
}

// -------- spfcos using octred then lock --------
spf spfcos(a)
spf a;
{
	spf ang;
	spf s0;
	spf c0;
	spf s;
	spf c;
	long oct;

	if(!cord_ok) cordin();
	ang = octred(a, &oct);
	if (locksp(ang, oct, &s0, &c0)) return c0;
	cord8(ang, &s, &c);
	octmap(&s0, &c0, s, c, oct);
	return c0;
}

// -------- spftan using octred then lock (plus pole guard) --------
spf spftan(a)
spf a;
{
	spf ang;
	spf s0;
	spf c0;
	spf s;
	spf c;
	spf ac;
	long oct;

	if(!cord_ok) cordin();
	ang = octred(a, &oct);
	if (locksp(ang, oct, &s0, &c0)) {
		if (spfeq(c0, spfz)) return (spfsgn(s0) > 0L) ? SPFPINF : SPFNINF;
		return spfdiv(s0, c0);
	}

	cord8(ang, &s, &c);
	octmap(&s0, &c0, s, c, oct);
	ac = spfabs(c0);
	if (spfcmp(ac, TANEPS) <= 0L) {
		long sgn;
		sgn = spfsgn(s0) * spfsgn(c0);
		return (sgn > 0L) ? SPFPINF : SPFNINF;
	}
	return spfdiv(s0, c0);
}

// ---- tangent, cotangent, secant, cosecant (SPF domain) ----

// cot(x) = cos(x)/sin(x)
spf spfcot(a)
spf a;
{
	spf s;
	spf c;
	long sign;
	s = spfsin(a);
	c = spfcos(a);
	// if |s| < EPSTINY -> +/-inf with correct sign
	if (spfcmp(s, EPSTINY) < 0 && spfcmp(s, spfneg(EPSTINY)) > 0) {
		sign = spfsgn(c) * spfsgn(s);
		return (sign < 0) ? SPFNINF : SPFPINF;
	}
	return spfdiv(c, s);
}

// sec(x) = 1/cos(x)
spf spfsec(a)
spf a;
{
	spf c;
	c = spfcos(a);
	// if |c| < EPSTINY -> +/-inf with sign of cos
	if (spfcmp(c, EPSTINY) < 0 && spfcmp(c, spfneg(EPSTINY)) > 0) {
		return (spfsgn(c) < 0) ? SPFNINF : SPFPINF;
	}
	return spfdiv(SPFONE, c);
}

// csc(x) = 1/sin(x)
spf spfcsc(a)
spf a;
{
	spf s;
	s = spfsin(a);
	// if |s| < EPSTINY -> +/-inf with sign of sin
	if (spfcmp(s, EPSTINY) < 0 && spfcmp(s, spfneg(EPSTINY)) > 0) {
		return (spfsgn(s) < 0) ? SPFNINF : SPFPINF;
	}
	return spfdiv(SPFONE, s);
}

