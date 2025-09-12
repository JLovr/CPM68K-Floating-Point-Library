/* ihyptt.c - Inverse hyperbolic function tests (CP/M-68K compliant)
 * - No brace initializers
 * - K&R function defs
 * - 7-bit ASCII, short identifiers
 * - Hex output for values
 *
 * Sections:
 *  1) ASNH: x = -6.0 .. +6.0 step 0.5; check sinh(asnh(x)) ~= x
 *  2) ACNH: x =  1.0 .. 20.0 step 0.5; check cosh(acnh(x)) ~= x
 *  3) ATNH: x = -0.99..+0.99 step ~0.11; check tanh(atnh(x)) ~= x
 */

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

/* fwd decls */
spf mkfrc();
void asntst();
void acntst();
void atntst();

/* make n/d as spf (both longs) */
spf mkfrc(n, d)
long n; long d;
{
    spf a, b, r;
    a = spfltf(n);
    b = spfltf(d);
    r = spfdiv(a, b);
    return r;
}

/* 1) ASNH: sinh(asnh(x)) ~= x, x in [-6, +6] step 0.5 */
void asntst()
{
    long i;
    spf xi, x, a, s, c, err;

    printf("ASNH: sinh(asnh(x)) ~= x   for x = -6.0 .. +6.0, step 0.5\n");
    printf("   x(hex)       asnh(x)      sinh(a)       err\n");
    printf("---------------------------------------------------\n");

    i = -12L;
    while (i <= 12L) {
        xi = spfltf(i);
        x  = spfmul(xi, spfhlf);     /* x = i * 0.5 */

        a = asnh(x);
        spfshc(a, &s, &c);           /* s = sinh(a) */
        err = spfsub(s, x);

        printf(" %08lX   %08lX   %08lX   %08lX\n",
               (unsigned long)x,
               (unsigned long)a,
               (unsigned long)s,
               (unsigned long)err);

        i = i + 1L;
    }
    printf("\n");
}

/* 2) ACNH: cosh(acnh(x)) ~= x, x in [1, 20] step 0.5 */
void acntst()
{
    long i;
    spf xi, x, a, s, c, err;

    printf("ACNH: cosh(acnh(x)) ~= x   for x = 1.0 .. 20.0, step 0.5\n");
    printf("   x(hex)       acnh(x)      cosh(a)       err\n");
    printf("---------------------------------------------------\n");

    i = 2L;                 /* 2 * 0.5 = 1.0 */
    while (i <= 40L) {      /* 40 * 0.5 = 20.0 */
        xi = spfltf(i);
        x  = spfmul(xi, spfhlf);     /* x = i * 0.5 */

        a = acnh(x);
        spfshc(a, &s, &c);           /* c = cosh(a) */
        err = spfsub(c, x);

        printf(" %08lX   %08lX   %08lX   %08lX\n",
               (unsigned long)x,
               (unsigned long)a,
               (unsigned long)c,
               (unsigned long)err);

        i = i + 1L;
    }
    printf("\n");
}

/* 3) ATNH: tanh(atnh(x)) ~= x, x in (-1,1) sampled ~0.11 */
void atntst()
{
    long i;
    spf x, a, t, err;

    printf("ATNH: tanh(atnh(x)) ~= x   for x ≈ -0.99 .. +0.99\n");
    printf("    x(hex)       atnh(x)      tanh(a)       err\n");
    printf("---------------------------------------------------\n");

    i = -99L;
    while (i <= 99L) {
        x = mkfrc(i, 100L);          /* x = i / 100 */
        a = atnh(x);
        t = spftnh(a);
        err = spfsub(t, x);

        printf(" %08lX   %08lX   %08lX   %08lX\n",
               (unsigned long)x,
               (unsigned long)a,
               (unsigned long)t,
               (unsigned long)err);

        i = i + 11L;                 /* ~0.11 step */
    }
    printf("\n");
}

WORD main()
{
    asntst();
    acntst();
    atntst();
    return 0;
}

