// MIT License
// Copyright (c) 2025 John J Lovrinic
// See LICENSE for details.

//--------------------------------------------------------
// spfioo.c
// Floating point to string routine for CP/M-68K
// single-precision floating point package
// Written by: J. Lovrinic
// Date: 20241028 (rev: fallback + rounding improvements)
//--------------------------------------------------------

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "spflib.h"

/* args:
   val    — spf to print
   buffer — output buffer pointer
   prec   — number of fractional digits to print (clamped 0..10)
   format — format character: 'e' or 'f'
*/

// --------------------------------------------------------
// ftoa: single-precision float -> string for CP/M-68K
//  - format 'f': fixed, but falls back to scientific if
//    decimal exponent y <= -4 or y >= 7 (classic thresholds)
//  - format 'e': scientific with two-digit exponent
//  - prec clamped to [0..10]
//  - handles -0 as 0
//  - one statement per line; K&R arg decls
// --------------------------------------------------------
char *ftoa(val, buffer, prec, format)
spf  val;
char *buffer;
long prec;
char format;
{
    long i;
    long j;
    long d;
    long y;
    long bits;
    long exp2;
    long iPART;
    long digit;
    long use_sci;
    long y_abs;

    spf  abs_val;
    spf  a;
    spf  ln_a;
    spf  term2;
    spf  sum;
    spf  ratio;
    spf  scaled;
    spf  power;
    spf  fPART;
    spf  expf;

    char intbuf[12];
    char digbuf[12];

    if (prec > 10L) prec = 10L;
    if (prec < 0L)  prec = 0L;

    if (spfeq(val, spfz) || spfeq(val, spfnz)) {
        i = 0L;
        buffer[i++] = '0';
        if (prec > 0L) {
            buffer[i++] = '.';
            for (j = 0L; j < prec; j++) buffer[i++] = '0';
        }
        buffer[i] = '\0';
        return buffer;
    }

    abs_val = spfabs(val);
    bits = abs_val;
    exp2 = ((bits >> 23L) & 0x000000FFL) - 127L;
    a = (bits & 0x007FFFFFL) | 0x3F800000L;

    ln_a = spfln(a);
    term2 = spfmul(spfltf(exp2), spfl2);
    sum = spfadd(ln_a, term2);
    ratio = spfdiv(sum, spfl10);
    y = spflng(ratio);

    use_sci = 0L;
    if (format == 'e') use_sci = 1L;
    if (format == 'f' && (y <= -4L || y >= 7L)) use_sci = 1L;

    if (use_sci) {
        expf = spfmul(spfltf(y), spfl10);
        power = spfexp(expf);
        scaled = spfdiv(abs_val, power);
    } else {
        scaled = abs_val;
    }

    iPART = spflng(scaled);
    fPART = spfsub(scaled, spfltf(iPART));

    for (d = 0L; d <= prec; d++) {
        fPART = spfmul(fPART, spf10);
        digit = spflng(fPART);
        digbuf[d] = (char)digit;
        fPART = spfsub(fPART, spfltf(digit));
    }

    if (digbuf[prec] >= 5) {
        for (d = prec - 1; d >= 0; d--) {
            digbuf[d] = (char)(digbuf[d] + 1);
            if (digbuf[d] < 10) break;
            digbuf[d] = 0;
        }
        if (d < 0) iPART = iPART + 1L;
    }

    if (use_sci && iPART >= 10L) {
        iPART = 1L;
        y = y + 1L;
    }

    i = 0L;
    if ((val & 0x80000000L) != 0L && !spfeq(val, spfnz)) buffer[i++] = '-';

    j = 0L;
    do {
        intbuf[j++] = (char)('0' + (iPART % 10L));
        iPART = iPART / 10L;
    } while (iPART);
    while (j--) buffer[i++] = intbuf[j];

    if (prec > 0L) {
        buffer[i++] = '.';
        for (d = 0L; d < prec; d++) buffer[i++] = (char)('0' + digbuf[d]);
    }

    if (use_sci) {
        buffer[i++] = 'e';
        buffer[i++] = (y >= 0L) ? '+' : '-';
        y_abs = (y >= 0L) ? y : -y;
        buffer[i++] = (char)('0' + ((y_abs / 10L) % 10L));
        buffer[i++] = (char)('0' + (y_abs % 10L));
    }

    buffer[i] = '\0';
    return buffer;
}

