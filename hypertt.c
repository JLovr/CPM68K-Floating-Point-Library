/* hypertt.c — hyperbolic function sweep & identities test
 *
 * Prints in hex; includes REL column for (t - s/c) and max abs/rel errors.
 * K&R style; no brace initializers; 6-char helper names.
 */

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

/* constants as SPF bit patterns */
#define X_MIN   0xC0C00000L  /* -6.0f */
#define X_MAX   0x40C00000L  /* +6.0f */
#define HALF    0x3F000000L  /* 0.5f  */
#define ONE     0x3F800000L  /* 1.0f  */
#define ZERO    0x00000000L

/* forward decls (old-style OK on this compiler) */
static void sweep();
static void idents();
static void parity();

static void sweep()
{
	long i;
	spf xi, x, s, c, t;

	/* iterate i = -12..+12, x = i * 0.5 */
	for (i = -12; i <= 12; i++) {
		xi = spfltf(i);
		x  = spfmul(xi, HALF);
		s  = spfsnh(x);
		c  = spfcoh(x);
		t  = spftnh(x);

		printf(" %08lX   %08lX   %08lX   %08lX\n",
		       (unsigned long)x,
		       (unsigned long)s,
		       (unsigned long)c,
		       (unsigned long)t);
	}
}

static void idents()
{
	long i;
	spf xi, x, s, c, t;
	spf e1, e2;          /* signed absolute errors */
	spf a1, a2, rel;     /* |e1|, |e2|, relative error */
	spf max_a1, max_a2, max_rel;
	spf at, denom;

	max_a1 = ZERO;
	max_a2 = ZERO;
	max_rel = ZERO;

	for (i = -12; i <= 12; i++) {
		xi = spfltf(i);
		x  = spfmul(xi, HALF);

		s = spfsnh(x);
		c = spfcoh(x);
		t = spftnh(x);

		/* e1 = (c*c - s*s) - 1 */
		e1 = spfsub(spfsub(spfmul(c, c), spfmul(s, s)), spfone);

		/* e2 = t - (s/c) */
		e2 = spfsub(t, spfdiv(s, c));

		/* absolute errors */
		a1 = spfabs(e1);
		a2 = spfabs(e2);

		/* rel = |t - s/c| / max(1, |t|) */
		at = spfabs(t);
		denom = at;
		if (spfcmp(denom, spfone) < 0) denom = spfone;
		rel = spfdiv(a2, denom);

		/* track maxima */
		if (spfcmp(a1, max_a1) > 0) max_a1 = a1;
		if (spfcmp(a2, max_a2) > 0) max_a2 = a2;
		if (spfcmp(rel, max_rel) > 0) max_rel = rel;

		printf(" %08lX   %08lX      %08lX      %08lX\n",
		       (unsigned long)x,
		       (unsigned long)e1,
		       (unsigned long)e2,
		       (unsigned long)rel);
	}

	printf("\nMAX ABS ERR:\n");
	printf("  (c^2-s^2-1): %08lX\n", (unsigned long)max_a1);
	printf("  (t - s/c)  : %08lX\n", (unsigned long)max_a2);
	printf("MAX REL ERR (t - s/c): %08lX\n", (unsigned long)max_rel);
}

static void parity()
{
	long k;
	spf x, sx, cx, tx;
	spf s_err, c_err, t_err;

	for (k = -6; k <= 6; k++) {
		x  = spfltf(k);

		sx = spfsnh(x);
		cx = spfcoh(x);
		tx = spftnh(x);

		/* sinh is odd; cosh is even; tanh is odd */
		s_err = spfadd(spfsnh(spfneg(x)), sx);
		c_err = spfsub(spfcoh(spfneg(x)), cx);
		t_err = spfadd(spftnh(spfneg(x)), tx);

		printf(" %08lX   %08lX            %08lX            %08lX\n",
		       (unsigned long)x,
		       (unsigned long)s_err,
		       (unsigned long)c_err,
		       (unsigned long)t_err);
	}
}

int main()
{
	printf("HYP SWEEP: x = -6.0 .. +6.0, step 0.5\n");
	printf("   x(hex)      sinh(x)     cosh(x)     tanh(x)\n");
	printf("------------------------------------------------\n");
	sweep();

	printf("\nIDENTITIES (errors): x = -6.0 .. +6.0, step 0.5\n");
	printf("   x(hex)    (c^2-s^2-1)     (t - s/c)        REL(t-s/c)\n");
	printf("------------------------------------------------------------\n");
	idents();

	printf("\nPARITY (errors): x = -6..+6 at integer points\n");
	printf("   x(hex)    sinh(-x)+sinh(x)  cosh(-x)-cosh(x)  tanh(-x)+tanh(x)\n");
	printf("-------------------------------------------------------------------\n");
	parity();

	return 0;
}

