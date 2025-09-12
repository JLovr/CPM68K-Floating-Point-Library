/* EULER FUNCTIONS TEST (exp, ln, log10, log2, pow)
   - CP/M-friendly: K&R style, no brace initializers, ftoa everywhere.
*/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "spflib.h"

/* local helpers */
static void pr_hdr();
static void do_exp();
static void do_logs();
static void do_lnei();
static void do_elny();
static void do_powt();

static unsigned long spfhex(x)
spf x;
{
    /* Reinterpret bits of SPF as 32-bit unsigned */
    unsigned long u;
    u = *(&x);
    return u;
}

int main()
{
    pr_hdr();
    do_exp();
    do_logs();
    do_lnei();
    do_elny();
    do_powt();
    return 0;
}

/* print section header */
static void pr_hdr()
{
    printf("EULER FUNCTIONS TEST (exp, ln, log10, log2, pow)\n");
    printf("--------------------------------------------------------\n");
}

/* -------- EXP section -------- */
static void do_exp()
{
    spf x[9];
    spf e;
    char buf[40];
    int i;

    /* inputs: -5, -1, -0.5, 0, 0.5, 1, 2, 5, 10 (as SPF hex) */
    x[0] = 0xC0A00000L;
    x[1] = 0xBF800000L;
    x[2] = 0xBF000000L;
    x[3] = 0x00000000L;
    x[4] = 0x3F000000L;
    x[5] = 0x3F800000L;
    x[6] = 0x40000000L;
    x[7] = 0x40A00000L;
    x[8] = 0x41200000L;

    printf("\nEXP: x -> exp(x)\n");
    printf("  x(hex)      exp(x) (dec)    exp(hex)  \n");
    printf("--------------------------------------------\n");

    for (i = 0; i < 9; i++) {
        e = spfexp(x[i]);
        ftoa(e, buf, 10, 'f');
        printf("  %08lX    %s    %08lX  \n",
               spfhex(x[i]), buf, spfhex(e));
    }
    fflush(stdout);
}

/* -------- LOGS section -------- */
static void do_logs()
{
    spf y[7];
    spf lny, lg10, lg2;
    char b1[40], b2[40], b3[40];
    int i;

    /* inputs: ~1e-6, 0.1, 0.5, 1, 2, 10, e */
    y[0] = 0x358637BDL;   /* ~1e-6 */
    y[1] = 0x3DCCCCCDL;   /* 0.1 */
    y[2] = 0x3F000000L;   /* 0.5 */
    y[3] = 0x3F800000L;   /* 1.0 */
    y[4] = 0x40000000L;   /* 2.0 */
    y[5] = 0x41200000L;   /* 10.0 */
    y[6] = 0x402DF854L;   /* e */

    printf("\nLOGS: y -> ln(y), log10(y), log2(y)\n");
    printf("  y(hex)      ln(y) (dec)     hex         log10 (dec)     hex         log2 (dec)      hex       \n");
    printf("-------------------------------------------------------------------------------------\n");

    for (i = 0; i < 7; i++) {
        lny  = spfln(y[i]);
        lg10 = spflog(y[i]);
        lg2  = spflg2(y[i]);
        ftoa(lny,  b1, 10, 'f');
        ftoa(lg10, b2, 10, 'f');
        ftoa(lg2,  b3, 10, 'f');
        printf("  %08lX    %s   %08lX    %s   %08lX    %s   %08lX  \n",
               spfhex(y[i]),
               b1, spfhex(lny),
               b2, spfhex(lg10),
               b3, spfhex(lg2));
    }
    fflush(stdout);
}

/* -------- Identity: ln(exp(x)) ≈ x -------- */
static void do_lnei()
{
    spf x[5];
    spf ex, lx, err;
    char b1[40], b2[40], b3[40];
    int i;

    x[0] = 0xC0000000L;  /* -2.0 */
    x[1] = 0xBF800000L;  /* -1.0 */
    x[2] = 0x00000000L;  /*  0.0 */
    x[3] = 0x3F800000L;  /* +1.0 */
    x[4] = 0x40000000L;  /* +2.0 */

    printf("\nIDENTITY: ln(exp(x)) \x88 x\n");
    printf("  x(hex)      exp(x) (dec)    hex         ln(exp(x)) (dec)  hex         err (dec)       hex       \n");
    printf("--------------------------------------------------------------------------------------------\n");

    for (i = 0; i < 5; i++) {
        ex  = spfexp(x[i]);
        lx  = spfln(ex);
        err = spfsub(lx, x[i]);
        ftoa(ex,  b1, 10, 'f');
        ftoa(lx,  b2, 10, 'f');
        ftoa(err, b3, 10, 'f');
        printf("  %08lX    %s    %08lX    %s    %08lX    %s  %08lX  \n",
               spfhex(x[i]),
               b1, spfhex(ex),
               b2, spfhex(lx),
               b3, spfhex(err));
    }
    fflush(stdout);
}

/* -------- Identity: exp(ln(y)) ≈ y -------- */
static void do_elny()
{
    spf y[5];
    spf ly, ey, err;
    char b1[40], b2[40], b3[40];
    int i;

    y[0] = 0x3DCCCCCDL;  /* 0.1 */
    y[1] = 0x3F000000L;  /* 0.5 */
    y[2] = 0x3F800000L;  /* 1.0 */
    y[3] = 0x40000000L;  /* 2.0 */
    y[4] = 0x41200000L;  /* 10.0 */

    printf("\nIDENTITY: exp(ln(y)) \x88 y\n");
    printf("  y(hex)      ln(y) (dec)     hex         exp(ln(y)) (dec)  hex         err (dec)       hex       \n");
    printf("----------------------------------------------------------------------------------------------\n");

    for (i = 0; i < 5; i++) {
        ly  = spfln(y[i]);
        ey  = spfexp(ly);
        err = spfsub(ey, y[i]);
        ftoa(ly,  b1, 10, 'f');
        ftoa(ey,  b2, 10, 'f');
        ftoa(err, b3, 10, 'f');
        printf("  %08lX    %s   %08lX    %s      %08lX    %s  %08lX  \n",
               spfhex(y[i]),
               b1, spfhex(ly),
               b2, spfhex(ey),
               b3, spfhex(err));
    }
    fflush(stdout);
}

/* -------- POW tests: spfpow vs spfpowi (integer n) -------- */
static void do_powt()
{
    spf a[4];
    long n[7];
    spf p, pi;
    spf ni;
    char b1[40], b2[40], b3[40];
    int i, j;

    a[0] = 0x3F000000L;  /* 0.5 */
    a[1] = 0x3F800000L;  /* 1.0 */
    a[2] = 0x40000000L;  /* 2.0 */
    a[3] = 0x41200000L;  /* 10.0 */

    n[0] = -3L; n[1] = -2L; n[2] = -1L; n[3] = 0L; n[4] = 1L; n[5] = 2L; n[6] = 3L;

    printf("\nPOW: pow(a,n) vs powi(a,n)  (n integer)\n");
    printf("  a(hex)      n    pow(a,n) (dec)  hex         powi(a,n) (dec)  hex         err (dec)   \n");
    printf("--------------------------------------------------------------------------------------\n");

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 7; j++) {
            ni = spfltf(n[j]);
            p  = spfpow(a[i], ni);
            pi = spfpowi(a[i], n[j]);
            ftoa(p,  b1, 10, 'f');
            ftoa(pi, b2, 10, 'f');
            ftoa(spfsub(p, pi), b3, 10, 'f');
            printf("  %08lX    %ld   %s    %08lX    %s    %08lX    %s\n",
                   spfhex(a[i]),
                   n[j],
                   b1, spfhex(p),
                   b2, spfhex(pi),
                   b3);
        }
    }
    fflush(stdout);
}

