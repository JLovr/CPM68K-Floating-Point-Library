/* htrigt.c — lightweight hyperbolic trig sanity/regression tests
 *
 * Purpose:
 *   Quick checks for sinh/cosh/tanh without the full sweep in hypertt.c.
 *   - Basic value sweep at coarse points
 *   - Core identities:
 *       cosh^2(x) - sinh^2(x) = 1
 *       tanh(x) = sinh(x) / cosh(x)
 *   - Parity checks (sinh odd, cosh even, tanh odd)
 *   - Addition theorems:
 *       sinh(x±y) = sinh x cosh y ± cosh x sinh y
 *       cosh(x±y) = cosh x cosh y ± sinh x sinh y
 *       tanh(x±y) = (tanh x ± tanh y) / (1 ± tanh x tanh y)
 *
 * Style:
 *   K&R function defs, 6-char names, no brace initializers, hex output.
 */

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

/* SPF hex constants */
#define SPF_ZE   0x00000000L  /* 0.0f */
#define SPF_ON   0x3F800000L  /* 1.0f */
#define SPF_HF   0x3F000000L  /* 0.5f */
#define SPF_M1   0xBF800000L  /* -1.0f */
#define SPF_T2   0x40000000L  /* 2.0f  */
#define SPF_M2   0xC0000000L  /* -2.0f */
#define SPF_T4   0x40800000L  /* 4.0f  */
#define SPF_M4   0xC0800000L  /* -4.0f */

/* forward decls (old-style) */
static void bsweep();
static void ident1();
static void parity();
static void addths();

/* print a small sweep at x = -4, -2, -1, 0, +1, +2, +4 */
static void bsweep()
{
	spf x, s, c, t;

	printf("BASIC SWEEP: x in {-4,-2,-1,0,+1,+2,+4}\n");
	printf("   x(hex)      sinh(x)     cosh(x)     tanh(x)\n");
	printf("------------------------------------------------\n");

	x = SPF_M4; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	x = SPF_M2; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	x = SPF_M1; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	x = SPF_ZE; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	x = SPF_ON; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	x = SPF_T2; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	x = SPF_T4; s = spfsnh(x); c = spfcoh(x); t = spftnh(x);
	printf(" %08lX   %08lX   %08lX   %08lX\n",
	       (unsigned long)x,(unsigned long)s,(unsigned long)c,(unsigned long)t);

	printf("\n");
}

/* identities at x = -3..+3 step 1.0, plus the tanh = sinh/cosh check */
static void ident1()
{
	long k;
	spf x, s, c, t;
	spf e1, e2;
	spf a1, a2, max1, max2;

	max1 = SPF_ZE;
	max2 = SPF_ZE;

	printf("IDENTITIES (errors): x = -3 .. +3, step 1\n");
	printf("   x(hex)    (c^2-s^2-1)     (t - s/c)\n");
	printf("------------------------------------------\n");

	for (k = -3; k <= 3; k++) {
		x = spfltf(k);
		s = spfsnh(x);
		c = spfcoh(x);
		t = spftnh(x);

		e1 = spfsub(spfsub(spfmul(c, c), spfmul(s, s)), spfone);
		e2 = spfsub(t, spfdiv(s, c));

		a1 = spfabs(e1);
		a2 = spfabs(e2);

		if (spfcmp(a1, max1) > 0) max1 = a1;
		if (spfcmp(a2, max2) > 0) max2 = a2;

		printf(" %08lX   %08lX      %08lX\n",
		       (unsigned long)x,
		       (unsigned long)e1,
		       (unsigned long)e2);
	}

	printf("\nMAX ABS ERR:\n");
	printf("  (c^2-s^2-1): %08lX\n", (unsigned long)max1);
	printf("  (t - s/c)  : %08lX\n", (unsigned long)max2);
	printf("\n");
}

/* parity checks at x = -4..+4 step 1 */
static void parity()
{
	long k;
	spf x, s_err, c_err, t_err;

	printf("PARITY (errors): x = -4 .. +4, step 1\n");
	printf("   x(hex)    sinh(-x)+sinh(x)  cosh(-x)-cosh(x)  tanh(-x)+tanh(x)\n");
	printf("-------------------------------------------------------------------\n");

	for (k = -4; k <= 4; k++) {
		x     = spfltf(k);
		s_err = spfadd(spfsnh(spfneg(x)), spfsnh(x));
		c_err = spfsub(spfcoh(spfneg(x)), spfcoh(x));
		t_err = spfadd(spftnh(spfneg(x)), spftnh(x));

		printf(" %08lX   %08lX            %08lX            %08lX\n",
		       (unsigned long)x,
		       (unsigned long)s_err,
		       (unsigned long)c_err,
		       (unsigned long)t_err);
	}
	printf("\n");
}

/* addition theorems over a small grid:
   x = {-2,-1,0,1,2}, y = {-1,-0.5,0,0.5,1,1.5} */
static void addths()
{
	long i, j;
	spf xi, yi, x, y;
	spf sx, cx, tx, sy, cy, ty;
	spf sp, cp, tp;   /* direct: f(x+y) */
	spf sm, cm, tm;   /* direct: f(x-y) */
	spf rs, rc, rt;   /* rhs for + */
	spf rsm, rcm, rtm;/* rhs for - */
	spf es, ec, et;
	spf esm, ecm, etm;

	printf("ADDITION THEOREMS (errors): x in {-2,-1,0,1,2}, y in {-1,-0.5,0,0.5,1,1.5}\n");
	printf("  x(hex)  y(hex)   es=sh+-rhs      ec=ch+-rhs      et=th+-rhs\n");
	printf("-----------------------------------------------------------------\n");

	for (i = -4; i <= 4; i += 2) {
		xi = spfltf(i);
		x  = spfmul(xi, SPF_HF);

		sx = spfsnh(x);
		cx = spfcoh(x);
		tx = spftnh(x);

		for (j = -2; j <= 3; j++) {
			yi = spfltf(j);
			y  = spfmul(yi, SPF_HF);

			sy = spfsnh(y);
			cy = spfcoh(y);
			ty = spftnh(y);

			sp = spfsnh(spfadd(x, y));
			cp = spfcoh(spfadd(x, y));
			tp = spftnh(spfadd(x, y));

			sm = spfsnh(spfsub(x, y));
			cm = spfcoh(spfsub(x, y));
			tm = spftnh(spfsub(x, y));

			/* plus formulas */
			rs = spfadd(spfmul(sx, cy), spfmul(cx, sy));
			rc = spfadd(spfmul(cx, cy), spfmul(sx, sy));
			rt = spfdiv(spfadd(tx, ty), spfadd(spfone, spfmul(tx, ty)));

			es = spfsub(sp, rs);
			ec = spfsub(cp, rc);
			et = spfsub(tp, rt);

			printf(" %08lX %08lX  %08lX   %08lX   %08lX\n",
			       (unsigned long)x, (unsigned long)y,
			       (unsigned long)es, (unsigned long)ec, (unsigned long)et);

			/* minus formulas */
			rsm = spfsub(spfmul(sx, cy), spfmul(cx, sy));
			rcm = spfsub(spfmul(cx, cy), spfmul(sx, sy));
			rtm = spfdiv(spfsub(tx, ty), spfsub(spfone, spfmul(tx, ty)));

			esm = spfsub(sm, rsm);
			ecm = spfsub(cm, rcm);
			etm = spfsub(tm, rtm);

			printf("              (-) %08lX   %08lX   %08lX\n",
			       (unsigned long)esm, (unsigned long)ecm, (unsigned long)etm);
		}
	}
	printf("\n");
}

int main()
{
	printf("HTRIGT — quick hyperbolic trig checks\n\n");

	bsweep();
	ident1();
	parity();
	addths();

	return 0;
}

