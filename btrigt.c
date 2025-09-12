/* btrigt.c - Full-circle trig tests for CP/M-68K (no brace initializers)
 *
 * Sections:
 *  1) Sine/Cosine table for 0..360 degrees, step 3
 *  2) Tangent table for -90..+90 degrees, step 3 (skips +/-90)
 *
 * Prints angles (deg), radians (hex), and function outputs (hex).
 */

#include <ctype.h>
#include <stdio.h>
#include "spflib.h"

/* constants (IEEE-754 single as long) */
#define P180 0x43340000L   /* 180.0f */
#define STEP 3L            /* 3-degree increment */
#define EPSC 0x33800000L   /* ~2^-24, guard for cos ~ 0 in tan */

/* forward decls (K&R) */
spf d2rad();
void sctest();
void tntest();

/* deg -> radians: rad = deg * pi / 180 */
spf d2rad(d)
long d;
{
    spf sd, r;
    sd = spfltf(d);
    r  = spfmul(sd, spfpi);
    r  = spfdiv(r, (spf)P180);
    return r;
}

/* Sine/Cosine: 0..360 by 3 deg */
void sctest()
{
    long deg;
    spf a, s, c;

    printf("SINE/COSINE  (0..360 deg, step 3)\n");
    printf(" deg  rad(hex)   sin(spf)  cos(spf)\n");
    printf("------------------------------------\n");

    deg = 0L;
    while (deg <= 360L) {
        a = d2rad(deg);
        s = spfsin(a);
        c = spfcos(a);

        printf("%4ld  %08lX  %08lX  %08lX\n",
               deg,
               (unsigned long)a,
               (unsigned long)s,
               (unsigned long)c);

        deg = deg + STEP;
    }
    printf("\n");
}

/* Tangent: -90..+90 by 3 deg (skip +/-90) */
void tntest()
{
    long deg;
    spf a, s, c, t, absc;

    printf("TANGENT  (-90..+90 deg, step 3; skipping +/-90)\n");
    printf(" deg  rad(hex)   tan(spf)\n");
    printf("--------------------------\n");

    deg = -90L;
    while (deg <= 90L) {
        if (deg == -90L || deg == 90L) {
            deg = deg + STEP;
            continue;
        }

        a = d2rad(deg);
        s = spfsin(a);
        c = spfcos(a);

        absc = spfabs(c);
        if (absc < (spf)EPSC) {
            /* near vertical asymptote; print sign*Inf */
            if (s ^ c) {
                /* crude sign pick; rely on IEEE bit */
                t = 0xFF800000L;   /* -inf */
            } else {
                t = 0x7F800000L;   /* +inf */
            }
        } else {
            t = spfdiv(s, c);
        }

        printf("%4ld  %08lX  %08lX\n",
               deg,
               (unsigned long)a,
               (unsigned long)t);

        deg = deg + STEP;
    }
    printf("\n");
}

WORD main()
{
    sctest();
    tntest();
    return 0;
}

