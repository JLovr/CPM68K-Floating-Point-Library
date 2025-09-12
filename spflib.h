#ifndef SPFLIB_H
#define SPFLIB_H

// spflib.h — single-precision floating-point library for CP/M-68K
// Pointers to functions and useful constants.
// Written: J. Lovrinic — 20241031
//
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

// ——— Types ————————————————————————————————————————————————————————————————
// All underlying storage is 32-bit long; names indicate intended usage.

typedef long spf;   // IEEE-754 single stored as 32-bit int
typedef long q2f;   // Q2.30 fixed
typedef long q4f;   // Q4.28 fixed
typedef long q8f;   // Q8.24 fixed

typedef struct {
        long revs;
        q2f  val;
        q2f  x;
        q2f  y;
} CORDIC;

typedef struct {
        long hi;
        long lo;
} q2d;

// ——— Comparators (use with spfcmp) ————————————————————————————————
#define spflt(a,b)  (spfcmp(a,b) <  0)
#define spfgt(a,b)  (spfcmp(a,b) >  0)
#define spfle(a,b)  (spfcmp(a,b) <= 0)
#define spfge(a,b)  (spfcmp(a,b) >= 0)
#define spfne(a,b)  (spfcmp(a,b) != 0)
#define spfeq(a,b)  (spfcmp(a,b) == 0)

// ——— Core ops ————————————————————————————————————————————————————————
spf spfadd();   // add
spf spfmul();   // multiply
spf spfsub();   // subtract
spf spfdiv();   // divide
spf spfmod();   // modulus

// ——— Math support ————————————————————————————————————————————————
spf  spfabs();  // absolute value
spf  spfdtr();  // degrees -> radians
spf  spfrtd();  // radians -> degrees
spf  spfint();  // integer part (truncate toward 0)
spf  spfflr();  // floor
spf  spfrnd();  // round to nearest
spf  spfcl();   // ceiling
spf  spfsr28(); // bitwise sqrt for Q4.28
spf  spfgfr();  // fractional component
spf  spfltf();  // long -> spf
q2f  fp_q2();   // IEEE float -> Q2.30
spf  q2_fp();   // Q2.30 -> IEEE float
spf  q4_fp();   // Q4.28 -> IEEE float
q4f  fp_q4();   // IEEE float -> Q4.28
long spfipow(); // integer x to integer power n>=0
spf  spfpowi(); // spf^integer
char *ftoa();   // spf -> string
spf  atof();    // string -> spf
spf  spfneg();  // negate
long spflng();  // spf -> long (truncate)
spf  spfsqr();  // square root

// ——— Misc / elementary ————————————————————————————————————————————
long sra();     // 32-bit arithmetic right shift
void fpq2d();   // spf -> q2d
spf  q2d_fp();  // q2d -> spf

// ——— Trig ————————————————————————————————————————————————————————————
spf spfsin();   // sin
spf spfcos();   // cos
spf spftan();   // tan
spf spfatn();   // atan

// ——— Euler ———————————————————————————————————————————————————————————
spf spfexp();   // e^x
spf spfln();    // ln
spf spflog();   // log10
spf spflg2();   // log2
spf spfpow();   // x^y

// ——— Hyperbolic ————————————————————————————————————————————————————
void spfshc();  // compute sinh & cosh together
spf  spfsnh();  // sinh
spf  spfcoh();  // cosh
spf  spftnh();  // tanh
spf  asnh();    // asinh
spf  acnh();    // acosh
spf  atnh();    // atanh

// ——— Other helpers ————————————————————————————————————————————————
spf  spfa2();   // atan2-like from sin and cos
spf  spfhyp();  // hypot
spf  spfmdf();  // modf: returns frac, int part via *arg2
void spffxp();  // extract mantissa & exponent
spf  spfldp();  // compose from mantissa & exponent
spf  spfcps();  // copysign(x,y)
spf  spfscb();  // scalbn: x * 2^n

// ——— Constants (IEEE-754 single stored in 32-bit int) ———————————————
#define spfone  0x3f800000L  // 1
#define spftwo  0x40000000L  // 2
#define spfhlf  0x3f000000L  // 0.5
#define spfe    0x402df854L  // e ≈ 2.7182817
#define spf10   0x41200000L  // 10
#define spfth   0x3dcccccdL  // 0.1
#define spfl10  0x40135d8eL  // ln(10)
#define spfl2   0x3f317218L  // ln(2)
#define spfpi   0x40490fdbL  // π
#define spf2pi  0x40c90fdbL  // 2π
#define spfpi2  0x3fc90fdbL  // π/2
#define spf3pi2 0x4096cbe4L  // 3π/2
#define spfpi4  0x3f490fdbL  // π/4
#define spfz    0x00000000L  // +0
#define spfnz   0x80000000L  // -0
#define spfn1   0xbf800000L  // -1
#define spfNAN  0x7fc00000L  // NaN

// Booleans
#define true    0x00000001L
#define false   0x00000000L

// Other useful integer limits
#define MAXINT  0x7FFFFFFFL  //  2,147,483,647
#define MININT  0x80000000L  // -2,147,483,648

#endif // SPFLIB_H

