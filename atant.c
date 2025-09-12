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
// atant.c — tests for spfatn (atan)
// CP/M-68K K&R style; names ≤7 chars; ftoa for all decimals
//--------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "spflib.h"

static void phex(val)
spf val;
{
    printf("%08lX", (unsigned long)val);
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
    printf("ATAN TESTS (spfatn)\n");
    printf("------------------------------------------------\n\n");

    printf("INVERSION: tan(atan(x)) ~= x   (x = -3..+3 step 0.25)\n");
    printf("  x(hex)      atan(x) (dec)     hex         tan(a) (dec)      hex         err(hex)\n");
    printf("-------------------------------------------------------------------------------------\n");
}

static void swinv()
{
    spf x, a, t, err;
    long i;
    char s1[32], s2[32];

    for (i = -12; i <= 12; i++) {
        x = spfmul(spfltf(i), 0x3E800000L);   // 0.25
        a = spfatn(x);
        t = spftan(a);
        err = spfsub(t, x);

        pf(s1, a, 9L);
        pf(s2, t, 9L);

        printf("  "); phex(x);
        printf("   %-12s  ", s1); phex(a);
        printf("   %-12s  ", s2); phex(t);
        printf("   "); phex(err);
        printf("\n");
    }
    printf("\n");
}

static void swcmp()
{
    spf x, ax, invx, a1, a2, sum, tgt, sgn;
    long k, it;
    char s1[32];

    printf("COMPLEMENT: atan(x)+atan(1/x) ~= sign(x)*pi/2  (x={0.125..4096}, ±)\n");
    printf("  x(hex)      atan(x)+atan(1/x) (dec)   hex         target(hex)   err(hex)\n");
    printf("--------------------------------------------------------------------------------\n");

    x = 0x3E000000L; // 0.125
    for (k = 0; k < 13; k++) {
        for (it = 0; it < 2; it++) {
            sgn  = (it == 0) ? spfn1 : spfone;     // -1 or +1
            ax   = spfmul(sgn, x);
            invx = spfdiv(spfone, ax);            // 1/(sign*x) == sign*(1/x)

            a1 = spfatn(ax);
            a2 = spfatn(invx);
            sum = spfadd(a1, a2);

            tgt = spfmul( ((it==0)? spfn1 : spfone), spfpi2 );

            pf(s1, sum, 9L);
            printf("  "); phex(ax);
            printf("   %-18s  ", s1); phex(sum);
            printf("   "); phex(tgt);
            printf("   "); phex( spfsub(sum, tgt) );
            printf("\n");
        }
        x = spfmul(x, 0x40000000L); // *2
    }
    printf("\n");
}

static void swodd()
{
    spf x, a1, a2, sum;
    long i;

    printf("ODDNESS: atan(-x)+atan(x) ~= 0   (x = -3..+3 step 0.5)\n");
    printf("  x(hex)      sum(hex)\n");
    printf("---------------------------\n");

    for (i = -6; i <= 6; i++) {
        x = spfmul(spfltf(i), 0x3F000000L); // 0.5
        a1 = spfatn(spfneg(x));
        a2 = spfatn(x);
        sum = spfadd(a1, a2);
        printf("  "); phex(x);
        printf("   "); phex(sum);
        printf("\n");
    }
    printf("\n");
}

int main()
{
    headr();
    swinv();
    swcmp();
    swodd();
    return 0;
}

