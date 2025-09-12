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
// atan2t.c — tests for spfa2(y,x)  (atan2 equivalent)
// CP/M-68K K&R style; names ≤7 chars; ftoa for all decimals
//--------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "spflib.h"

static void phex(v)
spf v;
{
    printf("%08lX", (unsigned long)v);
}

static void pf(buf, v, prec)
char *buf;
spf v;
long prec;
{
    (void)ftoa(v, buf, prec, 'f');
}

static void headr()
{
    printf("ATAN2 TESTS (spfa2)\n");
    printf("--------------------------------------------------------\n\n");
}

static void swth()
{
    long deg;
    spf th;
    spf s;
    spf c;
    spf a;
    spf err;
    char bA[32];

    printf("ANGLE SWEEP: theta = -180..+180 deg, step 15\n");
    printf("  deg   theta(hex)    atan2(s,c) (dec)    hex           err(hex)\n");
    printf("-----------------------------------------------------------------\n");

    for (deg = -180; deg <= 180; deg += 15) {
        th = spfdtr(spfltf(deg));
        s  = spfsin(th);
        c  = spfcos(th);
        a  = spfa2(s, c);
        err = spfsub(a, th);

        pf(bA, a, 9L);

        printf(" %4ld   ", deg);
        phex(th);
        printf("   %-16s", bA);
        printf("  "); phex(a);
        printf("   "); phex(err);
        printf("\n");
    }
    printf("\n");
}

static void swrat()
{
    // grid over y in {-2,-1,-0.5,0,+0.5,+1,+2}, x in same but x≠0 for ratio check
    spf ys[7];
    spf xs[7];
    long i;
    long j;
    spf y;
    spf x;
    spf a;
    spf t;
    spf r;
    spf err;
    char bA[32];
    char bT[32];
    char bR[32];

    ys[0] = spfmul(spfn1, 0x40000000L);    /* -2 */
    ys[1] = spfn1;                         /* -1 */
    ys[2] = 0xBF000000L;                   /* -0.5 */
    ys[3] = spfz;                          /*  0 */
    ys[4] = 0x3F000000L;                   /* +0.5 */
    ys[5] = spfone;                        /* +1 */
    ys[6] = 0x40000000L;                   /* +2 */

    for (i = 0; i < 7; i++) xs[i] = ys[i];

    printf("RATIO CHECK: tan(atan2(y,x)) ~= y/x   (x != 0)\n");
    printf("  y(hex)        x(hex)        a=atan2(y,x) (dec)   hex           tan(a) (dec)       hex           y/x (dec)         hex           err(hex)\n");
    printf("-----------------------------------------------------------------------------------------------------------------------------------------\n");

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 7; j++) {
            x = xs[j];
            y = ys[i];
            if (spfeq(x, spfz) || spfeq(x, spfnz)) continue;

            a = spfa2(y, x);
            t = spftan(a);
            r = spfdiv(y, x);
            err = spfsub(t, r);

            pf(bA, a, 6L);
            pf(bT, t, 6L);
            pf(bR, r, 6L);

            printf("  "); phex(y);
            printf("   "); phex(x);
            printf("   %-18s  ", bA); phex(a);
            printf("   %-16s  ", bT); phex(t);
            printf("   %-16s  ", bR); phex(r);
            printf("  "); phex(err);
            printf("\n");
        }
    }
    printf("\n");
}

static void swaxes()
{
    spf a;
    spf y;
    spf x;
    char bA[32];

    printf("AXES / SPECIAL CASES\n");
    printf("  case                         y(hex)        x(hex)        atan2 (dec)       hex\n");
    printf("-----------------------------------------------------------------------------------\n");

    /* (0, +1) -> 0 */
    y = spfz;
    x = spfone;
    a = spfa2(y, x);
    pf(bA, a, 6L);
    printf("  (0,+1)                       "); phex(y); printf("   "); phex(x);
    printf("   %-16s  ", bA); phex(a); printf("\n");

    /* (0, -1) -> pi */
    y = spfz;
    x = spfn1;
    a = spfa2(y, x);
    pf(bA, a, 6L);
    printf("  (0,-1)                       "); phex(y); printf("   "); phex(x);
    printf("   %-16s  ", bA); phex(a); printf("\n");

    /* (+1, 0) -> +pi/2 */
    y = spfone;
    x = spfz;
    a = spfa2(y, x);
    pf(bA, a, 6L);
    printf("  (+1,0)                       "); phex(y); printf("   "); phex(x);
    printf("   %-16s  ", bA); phex(a); printf("\n");

    /* (-1, 0) -> -pi/2 */
    y = spfn1;
    x = spfz;
    a = spfa2(y, x);
    pf(bA, a, 6L);
    printf("  (-1,0)                       "); phex(y); printf("   "); phex(x);
    printf("   %-16s  ", bA); phex(a); printf("\n");

    /* (0,0): implementation-defined; just show what we get */
    y = spfz;
    x = spfz;
    a = spfa2(y, x);
    pf(bA, a, 6L);
    printf("  (0,0)                        "); phex(y); printf("   "); phex(x);
    printf("   %-16s  ", bA); phex(a); printf("\n");

    printf("\n");
}

int main()
{
    headr();
    swth();
    swrat();
    swaxes();
    return 0;
}

