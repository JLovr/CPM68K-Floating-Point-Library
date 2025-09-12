/* mixtt.c - Tests for modf, frexp/ldexp, copysign, scalbn, hypot
 * CP/M-68K compliant:
 * - No brace initializers
 * - K&R definitions
 * - Short identifiers (<=6)
 * - Hex output
 */

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

/* fwd decls */
spf mkfrc();
void mdftst();
void frxtst();
void cpstst();
void scltst();
void hyptst();

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

/* ------------------------------ */
/* 1) MODF tests (spfmdf)         */
/* ------------------------------ */
void mdftst()
{
    WORD i;
    spf x, ip, fr, chk, err;

    printf("MODF tests: x -> ip + fr (ip=trunc toward 0)\n");
    printf("     x(hex)       ip(hex)       fr(hex)       (ip+fr-x)\n");
    printf("--------------------------------------------------------\n");

    i = 0;
    while (i < (WORD)12) {
        /* Build a small menu of values (neg, pos, frac, ints) */
        if (i == 0)  { x = spfneg(spfadd(spfltf(3L), mkfrc(3L,4L))); }   /* -3.75 */
        else if (i == 1) { x = spfneg(spfadd(spfltf(2L), mkfrc(1L,2L))); } /* -2.5 */
        else if (i == 2) { x = spfneg(spfltf(1L)); }                      /* -1.0 */
        else if (i == 3) { x = spfneg(mkfrc(1L,8L)); }                    /* -0.125 */
        else if (i == 4) { x = 0x80000000L; }                             /* -0.0 (if supported) */
        else if (i == 5) { x = 0x00000000L; }                             /* +0.0 */
        else if (i == 6) { x = mkfrc(1L,8L); }                            /* +0.125 */
        else if (i == 7) { x = spfadd(spfltf(1L), mkfrc(1L,2L)); }        /* 1.5 */
        else if (i == 8) { x = spfltf(2L); }                              /* 2.0 */
        else if (i == 9) { x = spfadd(spfltf(10L), mkfrc(3L,4L)); }       /* 10.75 */
        else if (i == 10){ x = spfneg(spfadd(spfltf(0L), mkfrc(3L,4L))); }/* -0.75 */
        else { x = spfadd(spfltf(4L), mkfrc(1L,4L)); }                    /* 4.25 */

        fr = spfmdf(x, &ip);
        chk = spfadd(ip, fr);
        err = spfsub(chk, x);

        printf(" %08lX   %08lX   %08lX   %08lX\n",
               (unsigned long)x,
               (unsigned long)ip,
               (unsigned long)fr,
               (unsigned long)err);

        i = i + 1;
    }
    printf("\n");
}

/* ---------------------------------------------- */
/* 2) FREXP/ LDEXP roundtrip (spffxp, spfldp)     */
/*    Check x == ldexp(m,e) after frexp(x)->m,e   */
/* ---------------------------------------------- */
void frxtst()
{
    WORD i;
    spf x, m, xr;
    long e;

    printf("FREXP/LDEXP roundtrip: x -> (m,e) -> x\n");
    printf("        x(hex)          m(hex)     e    ldexp(m,e)      err\n");
    printf("----------------------------------------------------------------\n");

    i = 0;
    while (i < (WORD)12) {
        /* x set: +/-{0.75, 1.0, 1.25, 2.0, 3.5, 10.0} */
        if (i == 0)  { x = mkfrc(3L,4L); }                                  /* 0.75 */
        else if (i == 1) { x = spfltf(1L); }
        else if (i == 2) { x = spfadd(spfltf(1L), mkfrc(1L,4L)); }          /* 1.25 */
        else if (i == 3) { x = spfltf(2L); }
        else if (i == 4) { x = spfadd(spfltf(3L), mkfrc(1L,2L)); }          /* 3.5 */
        else if (i == 5) { x = spfltf(10L); }
        else if (i == 6) { x = spfneg(mkfrc(3L,4L)); }                       /* -0.75 */
        else if (i == 7) { x = 0x80000000L; }                                /* -0.0 */
        else if (i == 8) { x = spfneg(spfadd(spfltf(1L), mkfrc(1L,4L))); }  /* -1.25 */
        else if (i == 9) { x = spfneg(spfltf(2L)); }
        else if (i == 10){ x = spfneg(spfadd(spfltf(3L), mkfrc(1L,2L))); }  /* -3.5 */
        else { x = spfneg(spfltf(10L)); }

        spffxp(x, &m, &e);
        xr = spfldp(m, e);

        printf(" %08lX   %08lX  %4ld   %08lX   %08lX\n",
               (unsigned long)x,
               (unsigned long)m,
               e,
               (unsigned long)xr,
               (unsigned long)spfsub(xr, x));

        i = i + 1;
    }
    printf("\n");
}

/* ------------------------------ */
/* 3) COPYSIGN (spfcps)           */
/* ------------------------------ */
void cpstst()
{
    WORD i;
    spf x, y, r;

    printf("COPYSIGN tests: r = copysign(x,y)\n");
    printf("        x(hex)        y(hex)        r(hex)\n");
    printf("------------------------------------------------\n");

    i = 0;
    while (i < (WORD)8) {
        if (i == 0)  { x = spfltf(2L);      y = spfltf(3L); }         /* +,+ */
        else if (i == 1) { x = spfltf(2L);  y = spfneg(spfltf(3L)); } /* +,- */
        else if (i == 2) { x = spfneg(spfltf(2L)); y = spfltf(3L); }  /* -, + */
        else if (i == 3) { x = spfneg(spfltf(2L)); y = spfneg(spfltf(3L)); }
        else if (i == 4) { x = mkfrc(1L,4L); y = 0x80000000L; }       /* 0 sign */
        else if (i == 5) { x = 0x00000000L;  y = spfltf(5L); }        /* preserve signed zero? */
        else if (i == 6) { x = mkfrc(99L,100L); y = spfneg(mkfrc(1L,100L)); }
        else { x = spfneg(mkfrc(99L,100L)); y = mkfrc(1L,100L); }

        r = spfcps(x, y);

        printf(" %08lX   %08lX   %08lX\n",
               (unsigned long)x,
               (unsigned long)y,
               (unsigned long)r);

        i = i + 1;
    }
    printf("\n");
}

/* --------------------------------------------------------- */
/* 4) SCALBN tests (spfscb) vs frexp+ldexp recomposition     */
/*    r = scalbn(x,n); alt = ldexp(m, e0+n) after frexp(x)   */
/* --------------------------------------------------------- */
void scltst()
{
    WORD i;
    spf x, r, m, alt;
    long e0, n;

    printf("SCALBN tests: r = scalbn(x,n)  vs  alt = ldexp(m, e0+n)\n");
    printf("        x(hex)     n     r(hex)        alt(hex)       err\n");
    printf("----------------------------------------------------------------\n");

    /* test set: x in {0.75, 1.0, -1.0, 1.25, -0.125}; n in {-10,-4,0,4,10} */
    i = 0;
    while (i < (WORD)25) {
        /* pick x by block and n by row inside block */
        if (i < 5)       { x = mkfrc(3L,4L); }
        else if (i < 10) { x = spfltf(1L); }
        else if (i < 15) { x = spfneg(spfltf(1L)); }
        else if (i < 20) { x = spfadd(spfltf(1L), mkfrc(1L,4L)); }
        else             { x = spfneg(mkfrc(1L,8L)); }

        if ((i % 5) == 0) { n = -10L; }
        else if ((i % 5) == 1) { n = -4L; }
        else if ((i % 5) == 2) { n = 0L; }
        else if ((i % 5) == 3) { n = 4L; }
        else { n = 10L; }

        /* scalbn path */
        r = spfscb(x, n);

        /* alt path via frexp/ldexp */
        spffxp(x, &m, &e0);
        alt = spfldp(m, e0 + n);

        printf(" %08lX  %4ld  %08lX   %08lX   %08lX\n",
               (unsigned long)x,
               n,
               (unsigned long)r,
               (unsigned long)alt,
               (unsigned long)spfsub(r, alt));

        i = i + 1;
    }
    printf("\n");
}

/* ---------------------------------------------- */
/* 5) HYPOT tests (spfhyp) vs sqrt(a^2+b^2)       */
/* ---------------------------------------------- */
void hyptst()
{
    WORD i;
    spf a, b, h, s, aa, bb, sum;

    printf("HYPOT tests: h = hypot(a,b)  vs  s = sqrt(a^2+b^2)\n");
    printf("        a(hex)        b(hex)        h(hex)        s(hex)        err\n");
    printf("-----------------------------------------------------------------------\n");

    i = 0;
    while (i < (WORD)12) {
        /* pairs: (0,0), (3,4), (-3,4), (4,-3), (1,1), (10,0.5), etc. */
        if (i == 0)  { a = 0x00000000L; b = 0x00000000L; }
        else if (i == 1) { a = spfltf(3L); b = spfltf(4L); }
        else if (i == 2) { a = spfneg(spfltf(3L)); b = spfltf(4L); }
        else if (i == 3) { a = spfltf(4L); b = spfneg(spfltf(3L)); }
        else if (i == 4) { a = spfltf(1L); b = spfltf(1L); }
        else if (i == 5) { a = spfltf(10L); b = mkfrc(1L,2L); }
        else if (i == 6) { a = mkfrc(3L,10L); b = mkfrc(4L,10L); }     /* 0.3,0.4 */
        else if (i == 7) { a = spfneg(mkfrc(1L,4L)); b = mkfrc(1L,8L); }
        else if (i == 8) { a = spfltf(100L); b = spfltf(1L); }
        else if (i == 9) { a = spfneg(spfltf(5L)); b = spfneg(spfltf(12L)); }
        else if (i == 10){ a = mkfrc(141L,100L); b = mkfrc(99L,100L); }/* 1.41,0.99 */
        else { a = spfltf(2L); b = 0x00000000L; }

        h  = spfhyp(a, b);

        aa = spfmul(a, a);
        bb = spfmul(b, b);
        sum = spfadd(aa, bb);
        s  = spfsqr(sum);

        printf(" %08lX   %08lX   %08lX   %08lX   %08lX\n",
               (unsigned long)a,
               (unsigned long)b,
               (unsigned long)h,
               (unsigned long)s,
               (unsigned long)spfsub(h, s));

        i = i + 1;
    }
    printf("\n");
}

WORD main()
{
    mdftst();
    frxtst();
    cpstst();
    scltst();
    hyptst();
    return 0;
}

