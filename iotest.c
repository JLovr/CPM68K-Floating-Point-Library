/* iotest.c - Tests for atof (atof) and ftoa (CP/M-68K compliant)
 *
 * - No brace initializers
 * - K&R function defs
 * - 7-bit ASCII, short identifiers (<=6)
 * - Hex output for exact IEEE-754 views
 *
 * Sections:
 *   1) Parse:   strings -> spf (atof)
 *   2) Roundtrip fixed ('f'): x -> str -> x2  at prec = {0,3,6,9}
 *   3) Roundtrip sci   ('e'): x -> str -> x2  at prec = {0,3,6}
 *   4) Format demo: show ftoa('f') vs ftoa('e') strings
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "spflib.h"

/* limits */
#define NSTR 14
#define MAXS 40   /* per-string buffer size */

/* globals */
char strs[NSTR][MAXS];
char buf1[MAXS];
char buf2[MAXS];

/* fwd decls (K&R) */
void init();
void parse();
void rtf();
void rte();
void show();

/* init test strings without brace initializers */
void init()
{
    /* Safe, representative cases (no NaN; modest exponents) */
    strcpy(strs[0],  "0");
    strcpy(strs[1],  "-0");
    strcpy(strs[2],  "1");
    strcpy(strs[3],  "-1");
    strcpy(strs[4],  "3.1415926");
    strcpy(strs[5],  "-2.5");
    strcpy(strs[6],  "123456.0");
    strcpy(strs[7],  "0.0001234");
    strcpy(strs[8],  "-0.0001234");
    strcpy(strs[9],  "1e3");
    strcpy(strs[10], "-2.5e2");
    strcpy(strs[11], "6.022e23");
    strcpy(strs[12], "9.1093836e-31");
    strcpy(strs[13], "2.7182818");
}

/* Section 1: parse with atof */
void parse()
{
    WORD i;
    spf x;

    printf("PARSE (atof): string -> hex\n");
    printf("  idx  input                       value(hex)\n");
    printf("------------------------------------------------\n");

    i = 0;
    while (i < (WORD)NSTR) {
        x = atof(strs[i]);
        printf(" %4d  %-26s  %08lX\n",
               (int)i, strs[i], (unsigned long)x);
        i = i + 1;
    }
    printf("\n");
}

/* helper: roundtrip once with given prec and format; prints row */
void rtrow(s, prec, fmt)
char *s; long prec; char fmt;
{
    spf x, y, d;

    x = atof(s);
    ftoa(x, buf1, prec, fmt);
    y = atof(buf1);
    d = spfsub(y, x);

    printf("  %-26s  p=%ld %c  out=\"%s\"  x:%08lX  y:%08lX  dy:%08lX\n",
           s, prec, (int)fmt, buf1,
           (unsigned long)x, (unsigned long)y, (unsigned long)d);
}

/* Section 2: roundtrip in fixed format */
void rtf()
{
    WORD i;
    long p;

    printf("ROUNDTRIP fixed ('f')\n");
    printf("  input                      prec  out              x(hex)     y(hex)     dy(hex)\n");
    printf("-----------------------------------------------------------------------------------\n");

    i = 0;
    while (i < (WORD)NSTR) {
        p = 0L;  rtrow(strs[i], p, 'f');
        p = 3L;  rtrow(strs[i], p, 'f');
        p = 6L;  rtrow(strs[i], p, 'f');
        p = 9L;  rtrow(strs[i], p, 'f');
        i = i + 1;
    }
    printf("\n");
}

/* Section 3: roundtrip in scientific format */
void rte()
{
    WORD i;
    long p;

    printf("ROUNDTRIP scientific ('e')\n");
    printf("  input                      prec  out              x(hex)     y(hex)     dy(hex)\n");
    printf("-----------------------------------------------------------------------------------\n");

    i = 0;
    while (i < (WORD)NSTR) {
        p = 0L;  rtrow(strs[i], p, 'e');
        p = 3L;  rtrow(strs[i], p, 'e');
        p = 6L;  rtrow(strs[i], p, 'e');
        i = i + 1;
    }
    printf("\n");
}

/* Section 4: side-by-side format demo for a few values */
void show()
{
    WORD i;
    spf x;

    printf("FORMAT DEMO: ftoa 'f' vs 'e' (prec=6)\n");
    printf("  input                      ftoa('f')               ftoa('e')\n");
    printf("---------------------------------------------------------------\n");

    i = 0;
    while (i < (WORD)NSTR) {
        x = atof(strs[i]);
        ftoa(x, buf1, 6L, 'f');
        ftoa(x, buf2, 6L, 'e');
        printf("  %-26s  %-22s  %-22s\n", strs[i], buf1, buf2);
        i = i + 1;
    }
    printf("\n");
}

WORD main()
{
    init();
    parse();
    rtf();
    rte();
    show();
    return 0;
}

