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
// Copyright (c) 2025 John J Lovrinic

#include <ctype>
#include <stdio.h>
#include <string.h>
#include "spflib.h"


// deg -> rad (π/180) in IEEE single
#define DEG2RAD 0x3C8EFA35L

int main()
{
    // test points: 0,3,6,...,45 degrees (16 samples)
    spf atab[16];
    spf ttab[16];
    spf ang;
    spf xin;
    spf r;      // spfatn(x)
    spf err;
    int i;

    // reference angles in radians (IEEE spf)
    for (i = 0; i < 16; ++i)
    {
        ang = spfmul(spfltf((long)(i * 3)), DEG2RAD);
        atab[i] = ang;
    }

    // IEEE tangent table for 0..45° every 3° (not using sin/cos to avoid coupling)
    ttab[0]  = 0x00000000L; // 0°
    ttab[1]  = 0x3D56A98AL; // 3°
    ttab[2]  = 0x3DD740E4L; // 6°
    ttab[3]  = 0x3E222F88L; // 9°
    ttab[4]  = 0x3E59A86DL; // 12°
    ttab[5]  = 0x3E8930A3L; // 15°
    ttab[6]  = 0x3EA65BE0L; // 18°
    ttab[7]  = 0x3EC489D4L; // 21°
    ttab[8]  = 0x3EE3F504L; // 24°
    ttab[9]  = 0x3F027043L; // 27°
    ttab[10] = 0x3F13CD3AL; // 30°
    ttab[11] = 0x3F263F93L; // 33°
    ttab[12] = 0x3F39FEB1L; // 36°
    ttab[13] = 0x3F4F4E02L; // 39°
    ttab[14] = 0x3F6680E1L; // 42°
    ttab[15] = 0x3F800000L; // 45°

    printf("ATAN 0..45 deg (step 3)\n");
    printf("  deg      atan(x)     ref(rad)    err\n");
    printf("-----------------------------------------------\n");

    for (i = 0; i < 16; ++i)
    {
        xin = ttab[i];
        r = spfatn(xin);
        err = spfsub(r, atab[i]);

        printf("%4d   %08lX   %08lX   %08lX\n",
               (int)(i * 3),
               (unsigned long)r,
               (unsigned long)atab[i],
               (unsigned long)err);
    }

    return 0;
}

